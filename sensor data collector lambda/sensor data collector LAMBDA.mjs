import { DynamoDBClient } from "@aws-sdk/client-dynamodb";
import { QueryCommand, PutCommand, DynamoDBDocumentClient } from "@aws-sdk/lib-dynamodb";

// Inicializar cliente de DynamoDB
const client = new DynamoDBClient({});
const docClient = DynamoDBDocumentClient.from(client);

// Función para verificar si el usuario existe en DynamoDB
const checkUserInDynamoDB = async (userName) => {
    // Realizar la consulta solo por 'thing_name'
    const command = new QueryCommand({
        TableName: "user_thing", // Nombre de tu tabla
        KeyConditionExpression: "thing_name = :thing_name", // Filtrar por thing_name
        ExpressionAttributeValues: {
            ":thing_name": "Esp32-sensor", // El valor que buscas para thing_name
        },
    });

    try {
        const data = await docClient.send(command);

        // Si se encontraron elementos, revisar si el 'username' coincide
        if (data.Items && data.Items.length > 0) {
            const userFound = data.Items.find(item => item.username === userName);
            if (userFound) {
                return true; // Usuario encontrado
            }
        }
        return false; // Usuario no encontrado
    } catch (error) {
        console.error("Error al verificar el usuario en DynamoDB:", error);
        return false; // Si hay un error, consideramos que el usuario no es válido
    }
};

export const handler = async (event, context) => {
    console.log("Evento recibido:", JSON.stringify(event));

    // Verifica si es una invocación desde Alexa
    if (event.session?.application?.applicationId) {
        const intentName = event.request?.intent?.name;

        if (intentName === "ProvideUserNameIntent") {
            const userName = event.request?.intent?.slots?.UserName?.value;

            // Verificar si el nombre de usuario existe en DynamoDB
            const userExists = await checkUserInDynamoDB(userName);

            if (userExists) {
                return buildAlexaResponse(
                    ¡Bienvenido, ${userName}! Ahora puedes consultar tu frecuencia cardíaca o información del curso. ¿Qué deseas hacer?
                );
            } else {
                return buildAlexaResponse(
                    "Usuario incorrecto. Por favor, proporciona un nombre válido."
                );
            }
        }

        if (!intentName || intentName === "AMAZON.HelpIntent") {
            return buildAlexaResponse(
                "¡Hola! Por favor, proporciona tu nombre para continuar."
            );
        }

        switch (intentName) {
            case "GetCourseInfoIntent":
                return buildAlexaResponse(
                    "Bienvenido al curso de sistemas. Puedes consultar el BPM actual. ¿Te gustaría saber tu frecuencia cardíaca?"
                );

            case "GetHeartRateIntent":
                const bpmData = await getBPMFromDynamoDB();
                if (bpmData) {
                    const { bpm, timestamp } = bpmData;
                    const date = new Date(timestamp).toLocaleString();

                    let message = Tu frecuencia cardíaca actual es de ${bpm} latidos por minuto, registrada en ${date}.;
                    
                    if (bpm >= 60 && bpm <= 70) {
                        message += " Esto es considerado muy bajo.";
                    } else if (bpm >= 70 && bpm <= 110) {
                        message += " Esto está dentro del rango normal.";
                    } else if (bpm >= 110 && bpm <= 150) {
                        message += " Esto es considerado alto.";
                    } else {
                        message += " Este valor está fuera de los rangos esperados.";
                    }

                    return buildAlexaResponse(message);
                } else {
                    return buildAlexaResponse(
                        "Lo siento, no pude obtener tu frecuencia cardíaca en este momento."
                    );
                }

            default:
                return buildAlexaResponse("Lo siento, no entendí tu solicitud.");
        }
    }

    // Manejo de otros eventos (como guardar en DynamoDB)
    try {
        let timestamp;
        if (typeof event.timestamp === "number") {
            timestamp = new Date(event.timestamp).toISOString();
        } else if (typeof event.timestamp === "string") {
            timestamp = event.timestamp;
        } else {
            timestamp = new Date().toISOString();
        }

        if (!event.thing_name || !event.bpm) {
            console.error("Datos incompletos:", event);
            return {
                statusCode: 400,
                body: JSON.stringify({
                    error: "Datos incompletos. Se requieren: thing_name, bpm y opcionalmente timestamp.",
                }),
            };
        }

        const command = new PutCommand({
            TableName: "sensor_data",
            Item: {
                timestamp: timestamp,
                thing_name: event.thing_name,
                bpm: event.bpm,
            },
        });

        const response = await docClient.send(command);
        console.log("Datos almacenados en DynamoDB:", response);
        return { statusCode: 200, body: "Datos almacenados con éxito." };
    } catch (error) {
        console.error("Error al almacenar los datos en DynamoDB:", error);
        return {
            statusCode: 500,
            body: JSON.stringify({
                error: "Error al almacenar los datos en DynamoDB.",
                details: error.message,
            }),
        };
    }
};

// Función para obtener el BPM desde DynamoDB
const getBPMFromDynamoDB = async () => {
    const command = new QueryCommand({
        TableName: "sensor_data",
        KeyConditionExpression: "thing_name = :thing_name",
        ExpressionAttributeValues: {
            ":thing_name": "Esp32-sensor",
        },
        ScanIndexForward: false,
        Limit: 1,
    });

    try {
        const data = await docClient.send(command);
        if (data.Items && data.Items.length > 0) {
            const item = data.Items[0];
            return { bpm: item.bpm, timestamp: item.timestamp };
        }
        return null;
    } catch (error) {
        console.error("Error al obtener BPM desde DynamoDB:", error);
        return null;
    }
};

// Función para construir la respuesta de Alexa
const buildAlexaResponse = (text) => {
    return {
        version: "1.0",
        response: {
            outputSpeech: {
                type: "PlainText",
                text: text,
            },
            shouldEndSession: false,
        },
    };
};