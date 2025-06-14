// task_manager_project
// Полная реализация всех функций Менеджера Задач на C++ + SFML
// Один файл main.cpp

/**
 * @mainpage Task Manager
 * @file main.cpp
 * @brief Основные определения типов и структуры Task, используемые в TaskManager.
 */

#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <stack>
#include <fstream>
#include <ctime>
#include <nlohmann/json.hpp>
#include <sstream>
#include <iomanip>
#include <algorithm>

using json = nlohmann::json;

/**
 * @enum Priority
 * @brief Перечисление уровней приоритета задачи.
 */
enum class Priority { Low, Medium, High };

/**
 * @enum Status
 * @brief Перечисление состояний выполнения задачи.
 */
enum class Status { Active, Done };

/**
 * @brief Преобразует перечисление Priority в строку.
 * @param p Приоритет.
 * @return Строка, соответствующая приоритету.
 */
std::string priorityToString(Priority p) {
    switch (p) {
        case Priority::Low: return "Low";
        case Priority::Medium: return "Medium";
        case Priority::High: return "High";
    }
    return "Unknown";
}

/**
 * @brief Преобразует перечисление Status в строку.
 * @param s Статус.
 * @return Строка, соответствующая статусу.
 */
std::string statusToString(Status s) {
    return s == Status::Active ? "Active" : "Done";
}


/**
 * @brief Преобразует строку в соответствующее значение перечисления Priority.
 * @param str Строка "Low", "Medium", или "High".
 * @return Значение Priority.
 */
Priority stringToPriority(const std::string& str) {
    if (str == "Low") return Priority::Low;
    if (str == "Medium") return Priority::Medium;
    return Priority::High;
}


/**
 * @brief Преобразует строку в соответствующее значение перечисления Status.
 * @param str Строка "Active" или "Done".
 * @return Значение Status.
 */
Status stringToStatus(const std::string& str) {
    return str == "Active" ? Status::Active : Status::Done;
}


/**
 * @brief Возвращает цвет, связанный с приоритетом задачи.
 * @param p Приоритет задачи.
 * @return sf::Color, соответствующий приоритету.
 */
sf::Color priorityColor(Priority p) {
    switch (p) {
        case Priority::Low: return sf::Color(100, 200, 100);
        case Priority::Medium: return sf::Color(255, 215, 0);
        case Priority::High: return sf::Color(255, 100, 100);
    }
    return sf::Color::White;
}


/**
 * @struct Task
 * @brief Структура, описывающая одну задачу.
 */
struct Task {
    std::string title;
    std::string description;
    Priority priority;
    Status status;
    std::string deadline;
    std::vector<std::string> tags;
    

    /**
     * @brief Преобразует задачу в JSON-объект.
     * @return json, описывающий текущую задачу.
     */
    json to_json() const {
        return json{{"title", title},
                    {"description", description},
                    {"priority", priorityToString(priority)},
                    {"status", statusToString(status)},
                    {"deadline", deadline},
                    {"tags", tags}};
    }


    /**
     * @brief Загружает задачу из JSON-объекта.
     * @param j json-объект.
     * @return Task, инициализированный данными из JSON.
     */
    static Task from_json(const json& j) {
        Task t;
        t.title = j.at("title").get<std::string>();
        t.description = j.at("description").get<std::string>();
        t.priority = stringToPriority(j.at("priority"));
        t.status = stringToStatus(j.at("status"));
        t.deadline = j.at("deadline").get<std::string>();
        t.tags = j.at("tags").get<std::vector<std::string>>();
        return t;
    }
};


/**
 * @class User
 * @brief Класс, представляющий пользователя и его задачи.
 */
class User {
public:
    std::string name;                   ///< Имя пользователя.
    std::vector<Task> tasks;           ///< Список задач пользователя.
    std::stack<std::vector<Task>> history; ///< Стек истории изменений задач для отката.


     /**
     * @brief Конструктор по имени пользователя.
     * @param username Имя пользователя.
     */
    User(const std::string& username) : name(username) {}


     /**
     * @brief Добавляет новую задачу.
     * @param task Задача для добавления.
     */
    void add_task(const Task& task) {
        save_state();
        tasks.push_back(task);
    }


    /**
     * @brief Удаляет задачу по индексу.
     * @param index Индекс задачи.
     */
    void delete_task(size_t index) {
        if (index < tasks.size()) {
            save_state();
            tasks.erase(tasks.begin() + index);
        }
    }


    /**
     * @brief Редактирует задачу по индексу.
     * @param index Индекс задачи.
     * @param updated_task Обновленная задача.
     */
    void edit_task(size_t index, const Task& updated_task) {
        if (index < tasks.size()) {
            save_state();
            tasks[index] = updated_task;
        }
    }


     /**
     * @brief Сохраняет задачи пользователя в JSON-файл.
     */
    void save_to_file() const {
        json j;
        for (const auto& t : tasks) j.push_back(t.to_json());
        std::ofstream file(name + "_tasks.json");
        file << j.dump(4);
    }

    /**
     * @brief Загружает задачи пользователя из JSON-файла.
     */
    void load_from_file() {
        std::ifstream file(name + "_tasks.json");
        if (!file.is_open()) return;
        json j;
        file >> j;
        tasks.clear();
        for (const auto& item : j) tasks.push_back(Task::from_json(item));
    }

     /**
     * @brief Откатывает изменения задач до предыдущего состояния.
     */
    void undo() {
        if (!history.empty()) {
            tasks = history.top();
            history.pop();
        }
    }

    /**
     * @brief Ищет задачи по ключевому слову в заголовке или описании.
     * @param keyword Ключевое слово.
     * @return Вектор найденных задач.
     */
    std::vector<Task> search(const std::string& keyword) const {
        std::vector<Task> result;
        for (const auto& t : tasks) {
            if (t.title.find(keyword) != std::string::npos ||
                t.description.find(keyword) != std::string::npos) {
                result.push_back(t);
            }
        }
        return result;
    }

     /**
     * @brief Фильтрует задачи по тегу.
     * @param tag Название тега.
     * @return Вектор задач с указанным тегом.
     */
    std::vector<Task> filter_by_tag(const std::string& tag) const {
        std::vector<Task> result;
        for (const auto& t : tasks) {
            if (std::find(t.tags.begin(), t.tags.end(), tag) != t.tags.end()) {
                result.push_back(t);
            }
        }
        return result;
    }

    /**
     * @brief Получает статистику задач по приоритетам.
     * @return Отображение количества задач для каждого приоритета.
     */
    std::map<Priority, int> get_priority_stats() const {
        std::map<Priority, int> stats;
        for (const auto& task : tasks) {
            stats[task.priority]++;
        }
        return stats;
    }

    /**
     * @brief Создает календарь дедлайнов с подсчетом задач на каждый день.
     * @return Отображение количества задач по дате дедлайна.
     */
    std::map<std::string, int> get_deadline_calendar() const {
        std::map<std::string, int> calendar;
        for (const auto& task : tasks) {
            calendar[task.deadline]++;
        }
        return calendar;
    }

     /**
     * @brief Фильтрует задачи по статусу.
     * @param status Статус задачи (Active или Done).
     * @return Вектор задач с заданным статусом.
     */
    std::vector<Task> filter_by_status(Status status) const {
        std::vector<Task> result;
        for (const auto& task : tasks) {
            if (task.status == status) result.push_back(task);
        }
        return result;
    }

    /**
     * @brief Возвращает ссылку на вектор всех задач пользователя.
     * @return Ссылка на вектор задач.
     */
    std::vector<Task>& get_tasks() {
        return tasks;
    }

private: 
    /**
     * @brief Сохраняет текущее состояние задач в стек истории.
     */
    void save_state() {
        history.push(tasks);
    }
};

/**
 * @brief Проверяет, просрочен ли дедлайн.
 * 
 * @param deadline Строка с дедлайном в формате "YYYY-MM-DD HH:MM".
 * @return true если дедлайн в прошлом, иначе false.
 */
bool isOverdue(const std::string& deadline) {
    std::tm tm = {};
    std::istringstream ss(deadline);
    ss >> std::get_time(&tm, "%Y-%m-%d %H:%M");
    if (ss.fail()) return false;

    std::time_t deadline_time = std::mktime(&tm);
    return std::difftime(deadline_time, std::time(nullptr)) < 0;
}

/**
 * @brief Проверяет, истекает ли дедлайн в течение 24 часов.
 * 
 * @param deadline Строка с дедлайном в формате "YYYY-MM-DD HH:MM".
 * @return true если дедлайн наступит в течение 24 часов, иначе false.
 */
bool isUrgent(const std::string& deadline) {
    std::tm tm = {};
    std::istringstream ss(deadline);
    ss >> std::get_time(&tm, "%Y-%m-%d %H:%M");
    if (ss.fail()) return false;

    std::time_t deadline_time = std::mktime(&tm);
    std::time_t now = std::time(nullptr);
    double diff = std::difftime(deadline_time, now);
    return diff > 0 && diff <= 86400;
}

/**
 * @class InputField
 * @brief Класс для создания текстового поля ввода в интерфейсе SFML.
 */
class InputField {
public:
    sf::RectangleShape box; ///< Прямоугольник-рамка вокруг поля ввода.
    sf::Text label;         ///< Метка (название) поля.
    sf::Text inputText;     ///< Отображаемый текст, введённый пользователем.
    std::string content;    ///< Содержимое, введённое пользователем.
    bool active = false;    ///< Флаг активности поля (можно ли вводить текст).

    /**
     * @brief Конструктор поля ввода.
     * @param font Шрифт SFML для отрисовки текста.
     * @param labelStr Название поля (отображается сверху).
     * @param x Координата X позиции поля.
     * @param y Координата Y позиции поля.
     */
    InputField(const sf::Font& font, const std::string& labelStr, float x, float y) {
        label.setFont(font);
        label.setString(labelStr);
        label.setCharacterSize(14);
        label.setFillColor(sf::Color::Black);
        label.setPosition(x, y);

        box.setSize(sf::Vector2f(300, 24));
        box.setFillColor(sf::Color::White);
        box.setOutlineColor(sf::Color::Black);
        box.setOutlineThickness(1);
        box.setPosition(x, y + 18);

        inputText.setFont(font);
        inputText.setCharacterSize(14);
        inputText.setFillColor(sf::Color::Blue);
        inputText.setPosition(x + 4, y + 20);
    }

    /**
     * @brief Отрисовывает поле ввода на указанном окне.
     * @param window Окно SFML, на которое будет выведено поле.
     */
    void draw(sf::RenderWindow& window) {
        window.draw(label);
        window.draw(box);
        inputText.setString(content);
        window.draw(inputText);
    }

    /**
     * @brief Обрабатывает событие ввода (мышь и клавиатура).
     * @param event Событие SFML.
     */
    void handleEvent(sf::Event& event) {
        if (event.type == sf::Event::MouseButtonPressed) {
            active = box.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y);
        } else if (active && event.type == sf::Event::TextEntered) {
            if (event.text.unicode == 8 && !content.empty()) {
                content.pop_back();
            } else if (event.text.unicode >= 32 && content.size() < 50) {
                content += static_cast<char>(event.text.unicode);
            }
        }
    }

    /**
     * @brief Возвращает текущий введённый текст.
     * @return std::string Содержимое поля.
     */
    std::string getText() const {
        return content;
    }

    /**
     * @brief Устанавливает содержимое поля вручную.
     * @param text Текст для отображения.
     */
    void setText(const std::string& text) {
        content = text;
    }
};


/**
 * @class GUIApp
 * @brief Графический интерфейс пользователя (GUI) для управления задачами с использованием SFML.
 */
class GUIApp {
private:
    User& user; ///< Ссылка на объект пользователя с задачами.
    sf::Font font; ///< Шрифт для всех текстовых элементов.
    sf::RenderWindow window; ///< Главное окно приложения.
    std::vector<InputField> fields; ///< Поля ввода задачи.
    InputField tagFilterField; ///< Поле для фильтрации по тегу.
    InputField dateSortField; ///< Поле для сортировки по дате.
    sf::RectangleShape saveButton; ///< Кнопка сохранения задачи.
    sf::Text saveText; ///< Текст на кнопке сохранения.
    sf::RectangleShape calendarButton; ///< Кнопка переключения на календарь.
    sf::Text calendarText; ///< Текст на кнопке календаря.
    std::vector<sf::FloatRect> taskRects; ///< Прямоугольники задач для кликов.
    std::vector<sf::FloatRect> deleteRects; ///< Прямоугольники кнопок удаления задач.
    int editingIndex = -1; ///< Индекс редактируемой задачи, -1 если создаётся новая.
    float scrollOffset = 0; ///< Отступ прокрутки по вертикали.
    bool calendarView = false; ///< Флаг режима отображения календаря.

public:
    /**
     * @brief Конструктор GUIApp.
     * @param u Ссылка на объект пользователя, с которым будет связан GUI.
     */
    GUIApp(User& u)
        : user(u), window(sf::VideoMode(900, 700), "Task Manager GUI"),
          tagFilterField(font, "Filter by tag:", 480, 10),
          dateSortField(font, "Sort by date (asc/desc):", 480, 70) {

        font.loadFromFile("arial.ttf");

        fields.emplace_back(font, "Title:", 30, 30);
        fields.emplace_back(font, "Description:", 30, 90);
        fields.emplace_back(font, "Deadline (YYYY-MM-DD HH:MM):", 30, 150);

        std::time_t now = std::time(nullptr);
        std::tm* localTime = std::localtime(&now);
        std::ostringstream oss;
        oss << std::put_time(localTime, "%Y-%m-%d %H:%M");
        fields[2].setText(oss.str());

        fields.emplace_back(font, "Priority (Low/Medium/High):", 30, 210);
        fields.emplace_back(font, "Status (Active/Done):", 30, 270);
        fields.emplace_back(font, "Tags (comma-separated):", 30, 330);

        saveButton.setSize(sf::Vector2f(200, 40));
        saveButton.setPosition(30, 400);
        saveButton.setFillColor(sf::Color(100, 200, 255));

        saveText.setFont(font);
        saveText.setCharacterSize(18);
        saveText.setString("Save Task");
        saveText.setFillColor(sf::Color::White);
        saveText.setPosition(50, 405);

        calendarButton.setSize(sf::Vector2f(200, 40));
        calendarButton.setPosition(30, 460);
        calendarButton.setFillColor(sf::Color(150, 200, 150));

        calendarText.setFont(font);
        calendarText.setCharacterSize(18);
        calendarText.setString("Calendar Calendar View");
        calendarText.setFillColor(sf::Color::White);
        calendarText.setPosition(50, 465);
    }

    /**
     * @brief Запускает главный цикл приложения, обрабатывая события и отрисовывая интерфейс.
     */
    void run() {
        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed)
                    window.close();

                for (auto& f : fields)
                    f.handleEvent(event);
                tagFilterField.handleEvent(event);
                dateSortField.handleEvent(event);

                if (event.type == sf::Event::MouseWheelScrolled) {
                    scrollOffset -= event.mouseWheelScroll.delta * 20;
                    scrollOffset = std::max(0.f, scrollOffset);
                }

                if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                    for (size_t i = 0; i < deleteRects.size(); ++i) {
                        if (deleteRects[i].contains(event.mouseButton.x, event.mouseButton.y)) {
                            user.delete_task(i);
                            user.save_to_file();
                            editingIndex = -1;
                            break;
                        }
                    }

                    if (saveButton.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
                        if (editingIndex >= 0) {
                            updateTask(editingIndex);
                            editingIndex = -1;
                        } else {
                            saveTask();
                        }
                    }

                    if (calendarButton.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
                        openCalendarWindow();
                    }

                    for (size_t i = 0; i < taskRects.size(); ++i) {
                        if (taskRects[i].contains(event.mouseButton.x, event.mouseButton.y)) {
                            loadTaskToForm(i);
                            editingIndex = static_cast<int>(i);
                        }
                    }
                }
            }

            window.clear(sf::Color(245, 245, 245));
            for (auto& f : fields)
                f.draw(window);
            tagFilterField.draw(window);
            dateSortField.draw(window);
            window.draw(saveButton);
            window.draw(saveText);
            window.draw(calendarButton);
            window.draw(calendarText);

            if (calendarView)
                openCalendarWindow();
            else
                drawTaskList();

            window.display();
        }
    }

    /**
     * @brief Сохраняет новую задачу, введённую в поля ввода, в список пользователя.
     */
    void saveTask() {
        Task newTask = createTaskFromFields();
        user.add_task(newTask);
        user.save_to_file();
    }

    /**
     * @brief Обновляет существующую задачу, используя данные из полей.
     * @param index Индекс задачи, которую нужно обновить.
     */
    void updateTask(int index) {
        Task updatedTask = createTaskFromFields();
        user.edit_task(index, updatedTask);
        user.save_to_file();
    }

    /**
     * @brief Создаёт задачу на основе значений, введённых в поля ввода.
     * 
     * @return Task Объект задачи, сформированный из содержимого формы.
     */
    Task createTaskFromFields() {
        std::string title = fields[0].getText();
        std::string desc = fields[1].getText();
        std::string deadline = fields[2].getText();
        Priority prio = stringToPriority(fields[3].getText());
        Status stat = stringToStatus(fields[4].getText());
        std::string tags_raw = fields[5].getText();

        std::vector<std::string> tags;
        std::stringstream ss(tags_raw);
        std::string tag;
        while (std::getline(ss, tag, ',')) {
            tags.push_back(tag);
        }

        return Task{title, desc, prio, stat, deadline, tags};
    }

    /**
     * @brief Загружает задачу по индексу в форму редактирования.
     * 
     * @param index Индекс задачи в списке пользователя.
     */
    void loadTaskToForm(size_t index) {
        if (index >= user.get_tasks().size()) return;
        const Task& t = user.get_tasks()[index];
        fields[0].setText(t.title);
        fields[1].setText(t.description);
        fields[2].setText(t.deadline);
        fields[3].setText(priorityToString(t.priority));
        fields[4].setText(statusToString(t.status));
        std::string tagStr;
        for (size_t i = 0; i < t.tags.size(); ++i) {
            tagStr += t.tags[i];
            if (i < t.tags.size() - 1) tagStr += ",";
        }
        fields[5].setText(tagStr);
    }
    /**
     * @brief Отображает список задач с учётом фильтрации по тегу и сортировки по дате.
     * 
     * Также визуализирует цветовой индикатор дедлайна и кнопки удаления.
     * Заполняет taskRects и deleteRects для обработки нажатий мыши.
     */
    void drawTaskList() {
        taskRects.clear();
        deleteRects.clear();
        auto tasks = user.get_tasks();

        std::string tagFilter = tagFilterField.getText();
        std::string dateOrder = dateSortField.getText();

        if (dateOrder == "asc") {
            std::sort(tasks.begin(), tasks.end(), [](const Task& a, const Task& b) {
                return a.deadline < b.deadline;
            });
        } else if (dateOrder == "desc") {
            std::sort(tasks.begin(), tasks.end(), [](const Task& a, const Task& b) {
                return a.deadline > b.deadline;
            });
        }

        int startY = 130;
        int x = 480;

        for (size_t i = 0; i < tasks.size(); ++i) {
            const auto& t = tasks[i];
            if (!tagFilter.empty() && std::find(t.tags.begin(), t.tags.end(), tagFilter) == t.tags.end()) continue;

            std::string tagStr;
            for (size_t j = 0; j < t.tags.size(); ++j) {
                tagStr += "#" + t.tags[j];
                if (j < t.tags.size() - 1) tagStr += ", ";
            }

            std::string text = t.title + " | " + t.deadline + " | " + priorityToString(t.priority) + " | " +
                               statusToString(t.status) + " | Tags: " + tagStr;

            sf::Text taskText(text, font, 14);
            taskText.setPosition(x + 20, startY - scrollOffset);
            taskText.setFillColor(sf::Color::Black);
            window.draw(taskText);

            sf::CircleShape statusCircle(5);
            statusCircle.setPosition(x + 5, startY - scrollOffset + 5);
            if (isOverdue(t.deadline)) {
                statusCircle.setFillColor(sf::Color::Red);
            } else if (isUrgent(t.deadline)) {
                statusCircle.setFillColor(sf::Color::Yellow);
            } else {
                statusCircle.setFillColor(sf::Color::Green);
            }
            window.draw(statusCircle);

            sf::Text delIcon("[X]", font, 16);
            delIcon.setFillColor(sf::Color(200, 50, 50));
            delIcon.setPosition(x - 20, startY - scrollOffset);
            window.draw(delIcon);
            deleteRects.push_back(delIcon.getGlobalBounds());

            sf::FloatRect rect(x, startY - scrollOffset, 400, 20);
            taskRects.push_back(rect);
            startY += 24;
        }
    }
    /**
     * @brief Открывает окно календаря с отображением количества задач по датам.
     */
    void openCalendarWindow();


};
    /**
     * @brief Открывает отдельное окно-календарь, отображающее задачи пользователя по дням.
     * 
     * Визуализирует задачи в виде календарной сетки, где каждый день может содержать несколько задач.
     * Календарь автоматически группирует задачи по году, месяцу и дню на основе дедлайнов.
     * Поддерживает переключение между доступными месяцами с задачами клавишами ← / →.
     * 
     * Особенности:
     * - Отображение названий месяцев и лет;
     * - Построение ячеек с названиями дней недели;
     * - Рисование задач внутри ячеек соответствующих дней;
     * - Подсказка по переключению месяцев;
     * - Обработка закрытия окна и переключения месяцев;
     * 
     * Использует:
     * - `sf::RenderWindow` для отрисовки;
     * - `std::map` для группировки задач;
     * - `std::tm` и `std::get_time` для разбора строк даты.
     * 
     * @note Пропускает задачи с некорректным форматом дедлайна.
     * @note Требует корректной инициализации `sf::Font font` в классе.
     * @see User::get_tasks()
     */
    void GUIApp::openCalendarWindow() {
        sf::RenderWindow calendarWindow(sf::VideoMode(900, 700), "Task Calendar");

        const std::vector<std::string> monthNames = {
            "January", "February", "March", "April", "May", "June",
            "July", "August", "September", "October", "November", "December"
        };

        std::map<int, std::map<int, std::map<int, std::vector<Task>>>> calendarData;

        for (const auto& task : user.get_tasks()) {
            std::tm tm = {};
            std::istringstream ss(task.deadline);
            ss >> std::get_time(&tm, "%Y-%m-%d %H:%M");
            if (ss.fail()) continue;

            int year = tm.tm_year + 1900;
            int month = tm.tm_mon;
            int day = tm.tm_mday;
            calendarData[year][month][day].push_back(task);
        }

        // Собираем все (год, месяц)
        std::vector<std::pair<int, int>> availableMonths;
        for (const auto& [year, months] : calendarData)
            for (const auto& [month, _] : months)
                availableMonths.emplace_back(year, month);

        std::sort(availableMonths.begin(), availableMonths.end());
        int currentIndex = 0;

        while (calendarWindow.isOpen()) {
            sf::Event event;
            while (calendarWindow.pollEvent(event)) {
                if (event.type == sf::Event::Closed)
                    calendarWindow.close();
                if (event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::Left)
                        currentIndex = std::max(0, currentIndex - 1);
                    if (event.key.code == sf::Keyboard::Right)
                        currentIndex = std::min((int)availableMonths.size() - 1, currentIndex + 1);
                }
            }

            calendarWindow.clear(sf::Color::White);

            const int cellW = 100, cellH = 80;
            const std::vector<std::string> days = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};
            for (int i = 0; i < 7; ++i) {
                sf::Text dayText(days[i], font, 16);
                dayText.setFillColor(sf::Color::Black);
                dayText.setPosition(50 + i * cellW, 20);
                calendarWindow.draw(dayText);
            }

            int yOffset = 60;

            int year = availableMonths[currentIndex].first;
            int month = availableMonths[currentIndex].second;

            sf::Text header("Month: " + monthNames[month] + " " + std::to_string(year), font, 24);
            header.setFillColor(sf::Color::Blue);
            header.setPosition(30, yOffset);
            calendarWindow.draw(header);
            yOffset += 30;

            std::tm firstDay = {};
            firstDay.tm_year = year - 1900;
            firstDay.tm_mon = month;
            firstDay.tm_mday = 1;
            std::mktime(&firstDay);
            int startWeekday = (firstDay.tm_wday + 6) % 7;

            int maxDays = 31;
            if (month == 1) {
                bool leap = (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0));
                maxDays = leap ? 29 : 28;
            } else if (month == 3 || month == 5 || month == 8 || month == 10) {
                maxDays = 30;
            }

            int row = 0;
            const auto& dayMap = calendarData[year][month];
            for (int day = 1; day <= maxDays; ++day) {
                int col = (startWeekday + day - 1) % 7;
                row = (startWeekday + day - 1) / 7;

                sf::RectangleShape cell(sf::Vector2f(cellW - 10, cellH - 10));
                cell.setFillColor(sf::Color(240, 240, 255));
                cell.setOutlineColor(sf::Color::Black);
                cell.setOutlineThickness(1);
                cell.setPosition(50 + col * cellW, yOffset + row * cellH);
                calendarWindow.draw(cell);

                sf::Text label("Day " + std::to_string(day), font, 14);
                label.setPosition(cell.getPosition().x + 5, cell.getPosition().y + 5);
                label.setFillColor(sf::Color::Black);
                calendarWindow.draw(label);

                if (dayMap.count(day)) {
                    for (size_t i = 0; i < dayMap.at(day).size(); ++i) {
                        sf::Text t(dayMap.at(day)[i].title, font, 12);
                        t.setFillColor(sf::Color::Black);
                        t.setPosition(cell.getPosition().x + 5, cell.getPosition().y + 25 + i * 15);
                        calendarWindow.draw(t);
                    }
                }
            }

            // Подсказка по переключению
            sf::Text hint("← / → to change month", font, 14);
            hint.setFillColor(sf::Color(100, 100, 100));
            hint.setPosition(30, 670);
            calendarWindow.draw(hint);

            calendarWindow.display();
        }
    }

/**
 * @brief Точка входа в программу.
 *
 * Запрашивает имя пользователя, загружает его задачи из файла и запускает графический интерфейс.
 *
 * Последовательность:
 * - Ввод имени пользователя через консоль;
 * - Создание экземпляра `User`;
 * - Загрузка задач из JSON-файла;
 * - Запуск графического интерфейса `GUIApp`.
 *
 * @return int Код завершения (0 при успешном завершении).
 *
 * @see User
 * @see GUIApp
 */
int main() {
    std::string username;
    std::cout << "Enter username: ";
    std::getline(std::cin, username);
    User user(username);
    user.load_from_file();
    GUIApp app(user);
    app.run();
    return 0;
}


