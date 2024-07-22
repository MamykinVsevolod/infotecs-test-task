#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

using namespace std;

// Функция принимает данные от клиента через сокет, проверяет сумму и выводит результат.
// Если соединение прерывается или возникает ошибка, закрывает сокет.
void analyzeData(int client_sock_fd)
{
    while (true)
    {
        char buffer[100];
        int bytesRead = recv(client_sock_fd, buffer, sizeof(buffer), 0);

        if (bytesRead > 0)
        {
            string sumStr(buffer);
            int sum = stoi(sumStr);
            cout << "Received sum: " << sum << endl;

            if (sumStr.length() > 2 && sum % 32 == 0)
            {
                cout << "The received sum is greater than 2 characters and is a multiple of 32." << endl;
            }
            else
            {
                cout << "Error: The received sum does not meet the criteria." << endl;
            }
        }
        else if (bytesRead == 0 || bytesRead == -1)
        {
            cerr << "Connection lost or error receiving data." << endl;
            close(client_sock_fd);
            return;
        }
    }
}

int main()
{
    // Создание сокета с использованием IPv4 и TCP. Проверка на ошибки.
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd == -1)
    {
        cerr << "Error creating socket" << endl;
        return 1;
    }

    // Инициализация структуры адреса сервера.
    // INADDR_ANY означает, что сервер будет принимать подключения на всех доступных интерфейсах.
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Привязка сокета к указанному адресу и порту. Проверка на ошибки.
    if (bind(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        cerr << "Error binding socket" << endl;
        close(sock_fd);
        return 1;
    }

    // Перевод сокета в режим "прослушивания", чтобы он мог принимать входящие соединения.
    if (listen(sock_fd, 1) == -1)
    {
        cerr << "Error listening on socket" << endl;
        close(sock_fd);
        return 1;
    }

    // Ожидание и принятие входящих подключений.
    // При успешном принятии соединения происходит вызов функции analyzeData
    // для обработки данных от клиента.
    while (true)
    {
        cout << "Waiting for connection..." << endl;
        int client_sock_fd = accept(sock_fd, NULL, NULL);
        if (client_sock_fd == -1)
        {
            cerr << "Error accepting connection" << endl;
            continue;
        }

        cout << "Connection established." << endl;
        analyzeData(client_sock_fd);
    }

    close(sock_fd); // Закрытие сокета
    return 0;
}
