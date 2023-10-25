// Task.cpp 
// Разработал Чернухин Владислав Витальевич

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <chrono>
#include <thread>
#include <conio.h> // Для асинхронного ввода



// Класс, представляющий лифт
class Elevator {

    int numFloors;  // Количество этажей
    int currentFloor;  // Текущий этаж
    int direction;  // Направление движения (1 - вверх, -1 - вниз, 0 - остановлен)
    bool stopF = false;  // Флаг промежуточной остановки
    std::vector<int> cabinRequests;  // Очередь запросов на этажи



public:
    //Начальные условия (лифт находится на 3м этаже, пассажиров нет)
    Elevator(int numFloors) : numFloors(numFloors), currentFloor(1), direction(0) {}   



    // Метод для вызова лифта с этажа
    void callElevator(int requestedFloor) {
        if (requestedFloor == currentFloor) {
            std::cout << "Cabin already on floor " << currentFloor << ". Doors opened." << std::endl;
        }
        else {
            cabinRequests.push_back(requestedFloor);
            if (direction == 0) {
                direction = (requestedFloor > currentFloor) ? 1 : -1;
            }
        }
    }

    // Метод для входа в лифт и нажатия кнопки этажа
    void enterCabin(int targetFloor) {
        cabinRequests.push_back(targetFloor);

        if (direction == 0) {
            direction = (targetFloor > currentFloor) ? 1 : -1;
        }
    }

    // Метод, эмулирующий движение лифта
    void moveElevator() {

        if (direction == 1) {
            currentFloor++;
        }
        else if (direction == -1) {
            currentFloor--;
        }

        // Поиск и сравнение текущего этажа с очередью запросов 
        auto it = std::find(cabinRequests.begin(), cabinRequests.end(), currentFloor);

        if (it != cabinRequests.end()) {
            cabinRequests.erase(it);
            stopF = true;

            if (!cabinRequests.empty()) {
                 
                //поиска ближайшего большего значения в списке запросов и изменение направления движения лифта в сторону ближайшего запрашиваемого этажа
                if (direction == 1) {
                    int target = currentFloor;
                    auto  nextReqFloor = std::min_element(cabinRequests.begin(), cabinRequests.end(), [target](int a, int b) {
                        return a > target && (a < b || b <= target);
                        });
                    direction = (*nextReqFloor > currentFloor) ? 1 : -1;
                }
                //поиска ближайшего меньшего значения в списке запросов и изменение направления движения лифта в сторону ближайшего запрашиваемого этажа
                if (direction == -1) {
                    int target = currentFloor;
                    auto  nextReqFloor = std::max_element(cabinRequests.begin(), cabinRequests.end(), [target](int a, int b) {
                        return a < target && (a > b || b >= target);
                        });
                    direction = (*nextReqFloor > currentFloor) ? 1 : -1;
                }


            }
        }

        if (cabinRequests.empty()) {
            direction = 0;
            return;
        }


    }

    // Метод для вывода текущего состояния лифта
    void printStatus() {
        std::cout << "Cabin on floor " << currentFloor;

        if (direction == 1 && !stopF) {
            std::cout << ". Going up. <Press any key to enter new command>" << std::endl;
        }
        else if (direction == -1 && !stopF) {
            std::cout << ". Going down. <Press any key to enter new command>" << std::endl;
        }
        else if (direction != 0 && stopF) {
            std::cout << ". Stopped. Doors opened" << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(2)); // Подождать перемещения пассажиров
            std::cout << "Doors are closing" << std::endl;
            stopF = !stopF;
        }
        else if (direction == 0 && stopF) {
            std::cout << ". Stopped. Doors opened" << std::endl;
            stopF = !stopF;
        }
        else {
            std::cout << ". Stopped. Doors opened" << std::endl;
        }

    }

    // Метод, эмулирующий движение лифта со скоростью 1 этаж в секунду
    void moveWithSpeed() {
        while (!cabinRequests.empty()) {

            moveElevator();
            printStatus();

            if (_kbhit()) { // Проверяем, была ли нажата клавиша

                std::string command;
                std::cout << ">> ";
                std::cin >> command;

                if (command[0] == 'E') {
                    break;  // завершаем программу
                }

                if (command.size() < 2) {
                    std::cout << "Error. Undefiend command. \nUse command F + floor number to call elevator. Use C + floor number to enter the elevator. Use E to exit from simulation." << std::endl;
                    continue;
                }

                int enteredFloor = std::stoi(command.substr(1));

                if (std::cin.fail() || enteredFloor < 1 || enteredFloor > numFloors) {
                    std::cin.clear(); // Сброс ошибки ввода
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Очистка буфера ввода
                    std::cout << "Invalid entry. Please enter a natural number not greater than the number of floors." << std::endl;
                    continue;
                }
                else if (command[0] == 'F') {
                    int requestedFloor = enteredFloor;

                    callElevator(requestedFloor);
                    moveWithSpeed();  // Перемещаем лифт после нажатия кнопки "вызов"

                }
                else if (command[0] == 'C') {
                    int targetFloor = enteredFloor;

                    enterCabin(targetFloor);
                    moveWithSpeed();  // Перемещаем лифт
                }
                else {
                    std::cout << "Error. Undefiend command. \nUse command F + floor number to call elevator. Use C + floor number to enter the elevator. Use E to exit from simulation." << std::endl;
                    continue;
                }


            }
            std::this_thread::sleep_for(std::chrono::seconds(1)); // Подождать 1 секунду

        }
    }

};



int main() {
    int numFloors;  // Вводим число этажей, считая самый нижний этаж первым
    
    while (true) {
    std::cout << "Enter the number of floors: ";
    std::cin >> numFloors;
    
    if (std::cin.fail() || numFloors <= 0) {
        std::cin.clear(); // Сброс ошибки ввода
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Очистка буфера ввода
        std::cout << "Invalid entry. Please enter a natural number." << std::endl;
        }
    else {
        break; // Правильный ввод, выходим из цикла
        }
    }


    Elevator elevator(numFloors);

    std::cout << "Use command F + floor number to call elevator. Use C + floor number to enter the elevator. Use E to exit from simulation." << std::endl;

    elevator.printStatus();


    while (true) {

        std::string command;
        std::cout << ">> ";
        std::cin >> command;

        if (command[0] == 'E') {
            break;  // завершаем программу
        }

        if (command.size() < 2) {
            std::cout << "Error. Undefiend command. \nUse command F + floor number to call elevator. Use C + floor number to enter the elevator. Use E to exit from simulation." << std::endl;
            continue;
        }

        int enteredFloor = std::stoi(command.substr(1));

        if (std::cin.fail() || enteredFloor < 1 || enteredFloor > numFloors) {
            std::cin.clear(); // Сброс ошибки ввода
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Очистка буфера ввода
            std::cout << "Invalid entry. Please enter a natural number not greater than the number of floors." << std::endl;
            continue;
        }
        else if (command[0] == 'F') {
            int requestedFloor = enteredFloor;

            elevator.callElevator(requestedFloor);
            elevator.moveWithSpeed();  // Перемещаем лифт после нажатия кнопки "вызов"

        }
        else if (command[0] == 'C') {
            int targetFloor = enteredFloor;

            elevator.enterCabin(targetFloor);
            elevator.moveWithSpeed();  // Перемещаем лифт
        }
        else {
            std::cout << "Error. Undefiend command. \nUse command F + floor number to call elevator. Use C + floor number to enter the elevator. Use E to exit from simulation." << std::endl;
            continue;
        }


    }

    return 0;
}
