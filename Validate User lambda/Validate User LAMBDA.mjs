import { DynamoDBClient } from "@aws-sdk/client-dynamodb";
import { PutCommand, DynamoDBDocumentClient } from "@aws-sdk/lib-dynamodb";

// Inicializar cliente de DynamoDB
const client = new DynamoDBClient({});
const docClient = DynamoDBDocumentClient.from(client);

export const handler = async (event) => {
    console.log("Evento recibido:", JSON.stringify(event));

    // Extraer los valores de username y thing_name del evento
    const { username, thing_name } = event;

    // Validación de los parámetros
    if (!username || !thing_name) {
        console.error("Datos incompletos:", event);
        return {
            statusCode: 400,
            body: JSON.stringify({
                error: "Datos incompletos. Se requieren: username y thing_name.",
            }),
        };
    }

    // Parámetros para DynamoDB
    const params = {
        TableName: "user_thing", // Nombre de tu tabla
        Item: {
            thing_name: thing_name, // Nombre del dispositivo
            username: username,     // Nombre del usuario
            timestamp: Date.now(),  // Timestamp en milisegundos
        },
    };

    // Insertar datos en DynamoDB
    try {
        const command = new PutCommand(params);
        const response = await docClient.send(command);
        console.log("Datos almacenados en DynamoDB:", response);
        return {
            statusCode: 200,
            body: JSON.stringify({
                message: "Datos guardados correctamente.",
                data: params.Item,
            }),
        };
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
