#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <string>
#include <algorithm>
#include <cctype>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

using namespace std;

// Функция проверяет, состоит ли строка только из цифр и не превышает ли 64 символа
bool isValidInput(const string &input)
{
    if (input.size() > 64)
        return false;
    return all_of(input.begin(), input.end(), ::isdigit);
}

// Функция сортирует строку в убывающем порядке и заменяет четные цифры на "KB"
string processInput(const string &input)
{
    string result = input;
    sort(result.begin(), result.end(), greater<char>());
    for (size_t i = 0; i < result.size(); ++i)
    {
        if ((result[i] - '0') % 2 == 0)
        {
            result.replace(i, 1, "KB");
            i++;
        }
    }
    return result;
}

// Класс общего буфера для двух потоков
class SharedBuffer
{
public:
    SharedBuffer() : dataReady(false), sock_fd(-1) {} // Конструктор без параметров (по умолчанию) с инициализацией полей

    // Метод ожидает, пока данные не будут готовы, запрашивает ввод от пользователя,
    // обрабатывает строку и устанавливает флаг dataReady.
    // В конце происходит уведомленение другого потока о готовности данных.
    void setData()
    {
        while (true)
        {
            unique_lock<mutex> lock(mtx);
            cv.wait(lock, [this]
                    { return !dataReady; });

            string input;
            cout << "Enter a string of digits (up to 64 characters): ";
            cin >> input;

            if (!isValidInput(input))
            {
                cerr << "Invalid input. Please enter a string of digits up to 64 characters." << endl;
                continue;
            }

            string processedInput = processInput(input);

            buffer = processedInput;
            dataReady = true;

            cv.notify_one();
        }
    }

    // Метод ожидает, пока данные не будут готовы, затем вычисляет сумму чисел в строке,
    // очищает буфер и пытается отправить сумму на сервер.
    // Если отправка данных не удалась, он повторяет попытку повторного подключения к серверу.
    void getData()
    {
        while (true)
        {
            unique_lock<mutex> lock(mtx);
            cv.wait(lock, [this]
                    { return dataReady; });

            string data = buffer;
            cout << "Processed data: " << data << endl;

            int sum = 0;
            for (char c : data)
            {
                if (isdigit(c))
                {
                    sum += (c - '0');
                }
            }

            cout << "Sum of numerical values: " << sum << endl;
            buffer.clear();
            dataReady = false;

            while (true)
            {
                if (sendData(to_string(sum)))
                {
                    cout << "Data sent successfully." << endl;
                    break;
                }
                else
                {
                    cerr << "Failed to send data. Server might be down. Retrying..." << endl;
                    sleep(2); // Ожидание перед попыткой подключения
                    reconnect();
                }
            }

            cv.notify_one();
        }
    }

    // Устанавливает файловый дескриптор сокета.
    void setSocket(int socket_fd)
    {
        sock_fd = socket_fd;
    }

    // Метод закрывает существующйи сокет (если он есть), создает нвоый сокет и пытается
    // подключиться к серверу на порту 8080.
    void reconnect()
    {
        if (sock_fd != -1)
        {
            close(sock_fd);
        }

        sock_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (sock_fd == -1)
        {
            cerr << "Error creating socket" << endl;
            return;
        }

        sockaddr_in server_addr;
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(8080);
        server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

        if (connect(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
        {
            cerr << "Error connecting to server" << endl;
            close(sock_fd);
            sock_fd = -1;
        }
        else
        {
            cout << "Reconnected to the server." << endl;
        }
    }

private:
    // Отправляет данные с использованием сокета. Проверяет ошибки, связанные с отправкой,
    // а также состояние сокета после отправки данных.
    bool sendData(const string &data)
    {
        if (sock_fd != -1)
        {
            ssize_t bytesSent = send(sock_fd, data.c_str(), data.length() + 1, 0);
            if (bytesSent == -1)
            {
                cerr << "Error sending data: " << strerror(errno) << endl;
                if (errno == EPIPE || errno == ECONNRESET)
                {
                    return false;
                }
            }
            else if (bytesSent < data.length() + 1)
            {
                cerr << "Partial send, connection might be down." << endl;
                return false;
            }

            // Проверка состояния соединения после отправки
            int error = 0;
            socklen_t len = sizeof(error);
            int retval = getsockopt(sock_fd, SOL_SOCKET, SO_ERROR, &error, &len);
            if (retval != 0 || error != 0)
            {
                cerr << "Socket error detected: " << strerror(error) << endl;
                return false;
            }

            return true;
        }
        return false;
    }

    mutex mtx;
    condition_variable cv;
    string buffer;
    bool dataReady;
    int sock_fd;
};

int main()
{
    SharedBuffer sharedBuffer; // Создаем экземпляр класса общего буфера
    sharedBuffer.reconnect();  // Вызываем метод подключения к серверу

    // Создаем два потока
    thread t1(&SharedBuffer::getData, ref(sharedBuffer));
    thread t2(&SharedBuffer::setData, ref(sharedBuffer));

    // Главный поток (main) будет ожидать завершения выполнения потоков
    t1.join();
    t2.join();

    return 0;
}
