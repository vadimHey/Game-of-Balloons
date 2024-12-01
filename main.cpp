#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "mainMenu.hpp"
#include <vector>
#include <string>
#include <random>
#include <sstream>

//Структура для хранения информации о воздушном шарике
struct Balloon 
{
    RectangleShape shape; //Форма шарика
    float speed = 0; //Скорость движения
    char letter = ' '; //Буква на шарике
};

void setText(Text& text, Font& font, Color color, int size, float width, float height)
{
    text.setFont(font);
    text.setFillColor(color);
    text.setCharacterSize(size);
    text.setPosition(width, height);
}

void setTexture(RectangleShape& heart, int widthSize, int heightSize, float width, float height, Texture& texture)
{
    heart.setSize(Vector2f(widthSize, heightSize));
    heart.setPosition(width, height);
    heart.setTexture(&texture); 
}

int main()
{
    //Создание окна MENU
    RenderWindow MENU;
    MENU.create(VideoMode::getDesktopMode(), "Pennywise`s balloons");

    MainMenu mainMenu(MENU.getSize().x, MENU.getSize().y);

    //Размер экрана
    float width = VideoMode::getDesktopMode().width;
    float height = VideoMode::getDesktopMode().height;

    float balloonSpeed = 0.2f; //Скорость движения шарика
    Vector2f balloonPosition(width / 2.5f, height); //Начальное положение шарика

    mt19937 rng(static_cast<unsigned int>(time(0))); //Генератор случайных чисел
    uniform_int_distribution<int> widthRange(110, width - 110); //Положение по горизонтали относительно размера воздушных шариков
    uniform_real_distribution<float> speedRange(0.15f, 0.3f); //Диапазон скорости взлета воздушных шариков 
    uniform_int_distribution<int> letterRange(0, 25); //Диапазон случайных букв

    vector<Balloon> balloons; //Вектор для хранения шариков

    float spawnTime = 0.0f; //Переменная для управления временем появления новых шариков
    float spawnInterval = 13.0f; //Интервал появления шариков

    int score = 0; //Переменная для отслеживания счета
    int lives = 3; //Изначальное количество жизней в игре

    //Настройка параметров анимации
    const int frameWidth = 709;
    const int frameHeight = 115;
    const int countFrames = 8;
    const float frameDuration = 0.5f / countFrames;
    const int distanceBetweenFrames = 87;

    //Цикл обработки событий
    Clock clock;
    Clock reloadClock;
    float elapsed = 0.0f;
    int currentFrame = 0;

    //Устанавливаем спрайт перезарядки
    Texture reload;
    if (!reload.loadFromFile("involved/reload.png"))
        throw "No reload image is here";
    Sprite reloadSprite;
    reloadSprite.setTexture(reload);
    reloadSprite.setTextureRect(IntRect(0, 0, frameWidth, frameHeight));

    bool isReloading = false; //Переменная перезарядки 

    //Устанавливаем изображение для иконки
    Image icon; 
    if (!icon.loadFromFile("involved/icon.png"))
        throw "No icon image is here";
    MENU.setIcon(480, 480, icon.getPixelsPtr()); 

    //Загружаем 26 текстур воздушных шариков для каждой буквы
    vector<Texture> balloonTextures(26);
    for (int i = 0; i < 26; ++i) 
    {
        string filename = "involved/balloon_" + string(1, 'A' + i) + ".png"; //Имена воздушных шаров с буквами
        if (!balloonTextures[i].loadFromFile(filename)) 
            cout << "No texture image for letter " << static_cast<char>('A' + i) << " is here" << endl;
    }

    //Устанавливаем задний фон
    Texture backTexture;
    if (!backTexture.loadFromFile("involved/background.png"))
        throw "No background image is here";
    RectangleShape background;
    background.setSize(Vector2f(1920, 1080));
    background.setTexture(&backTexture);

    //Устанавливаем сердечки жизни в игре
    Texture heartTexture;
    if (!heartTexture.loadFromFile("involved/heart.png"))
        throw "No heart image is here";
    RectangleShape heart1, heart2, heart3;
    setTexture(heart1, 100, 100, width - 110, 10, heartTexture);
    setTexture(heart2, 100, 100, width - 210, 10, heartTexture);
    setTexture(heart3, 100, 100, width - 310, 10, heartTexture);
    
    //Устанавливаем изображения для пунктов меню
    Texture scoreTexture, heartsTexture, exitTexture;
    if (!scoreTexture.loadFromFile("involved/score.png"))
        throw "No score image is here";
    if (!heartsTexture.loadFromFile("involved/hearts.png"))
        throw "No hearts image is here";
    if (!exitTexture.loadFromFile("involved/exit.png"))
        throw "No exit image is here";
    RectangleShape scoreImage, heartsImage, exitImage;
    setTexture(scoreImage, 266, 100, width / 4.0, height / 12.0, scoreTexture);
    setTexture(heartsImage, 302, 100, width / 1.5, height / 12.0, heartsTexture);
    setTexture(exitImage, 444, 283, width / 1.8, height / 1.8, exitTexture);
    
    //Устанавливаем шрифт текста
    Font font;
    if (!font.loadFromFile("involved/font.ttf"))
        throw "No font for text is here";

    //Устанавливаем звук лопанья воздушных шариков и музыку на фон
    SoundBuffer popBuffer, musicBuffer;
    if (!popBuffer.loadFromFile("involved/pop.mp3")) 
        throw "No pop sound is here";
    Sound popSound(popBuffer);

    //Пока окно меню открыто
    while (MENU.isOpen())
    {
        Event event;
        while (MENU.pollEvent(event))
        {
            if (event.type == Event::Closed)
                MENU.close();
            
            if (event.type == Event::KeyReleased)
            {
                if (event.key.code == Keyboard::Up || event.key.code == Keyboard::W)
                {
                    mainMenu.moveUp();
                    break;
                }

                if (event.key.code == Keyboard::Down || event.key.code == Keyboard::S)
                {
                    mainMenu.moveDown();
                    break;
                }

                if (event.key.code == Keyboard::Return)
                {
                    RenderWindow play(VideoMode::getDesktopMode(), "game_play");
                    RenderWindow gamePurpose(VideoMode::getDesktopMode(), "game_purpose");
                    RenderWindow managementInformation(VideoMode::getDesktopMode(), "management_information");
                    RenderWindow authorData(VideoMode::getDesktopMode(), "author_data");

                    //Выбор пользователем пункта главного меню
                    int menuChoice = mainMenu.mainMenuPressed();

                    if (menuChoice == 0)
                    {
                        //Обнуляем счет и количество жизней
                        score = 0;
                        lives = 3;

                        //Объект для вывода счета в игре
                        Text scoreText;
                        setText(scoreText, font, Color::Black, 50, 10, 10);

                        //Объект для вывода сообщения об окончании игры
                        Text gameOverText;
                        setText(gameOverText, font, Color::Black, 60, width / 3.8, height / 2.4);
                        gameOverText.setString("Game Over! Press 'Escape' to return to menu");

                        //Обновляем текст с текущим счетом
                        stringstream ss;
                        ss << "Score: " << score;
                        scoreText.setString(ss.str());

                        bool gameOver = false; //Переменная для отслеживания окончания игры

                        balloons.clear(); //Обнуляем все шарики в векторе
                        play.clear(); //Очищаем игровое окно

                        //Отображаем начальное количество сердечек
                        play.draw(heart1);
                        play.draw(heart2);
                        play.draw(heart3);

                        while (play.isOpen())
                        {
                            Event eventChoice;
                            while (play.pollEvent(eventChoice))
                            {
                                if (eventChoice.type == Event::Closed)
                                    play.close();
                                else if (eventChoice.type == Event::KeyPressed)
                                {
                                    if (!gameOver && eventChoice.key.code == Keyboard::Escape)
                                    {
                                        //Обнуляем все шарики в векторе
                                        balloons.clear();

                                        //Очищаем игровое окно
                                        play.clear();

                                        //Пользователь нажал Escape, открываем окно подтверждения выхода
                                        RenderWindow confirmExit(VideoMode(400, 200), "Confirm Exit?");
                                        confirmExit.setPosition(Vector2i((VideoMode::getDesktopMode().width - 400) / 2, (VideoMode::getDesktopMode().height - 200) / 2));

                                        //Создаем текст сообщения в окне подтверждения выхода
                                        Text confirmText;
                                        setText(confirmText, font, Color::White, 24, 20, 50);
                                        confirmText.setString("Are you sure you want to exit?\nYour points will be reset to zero!\nPress 'Y' to confirm, 'N' to cancel.");

                                        //Отображаем текст в окне подтверждения выхода
                                        confirmExit.clear(Color::Black);
                                        confirmExit.draw(confirmText);
                                        confirmExit.display();

                                        //Цикл обработки событий окна подтверждения выхода
                                        bool exitConfirmed = false;
                                        while (!exitConfirmed && confirmExit.isOpen())
                                        {
                                            Event confirmEvent;
                                            while (confirmExit.pollEvent(confirmEvent))
                                            {
                                                if (confirmEvent.type == Event::Closed)
                                                    confirmExit.close(); // Закрыть окно подтверждения при закрытии окна
                                                else if (confirmEvent.type == Event::KeyPressed)
                                                {
                                                    if (confirmEvent.key.code == Keyboard::Y)
                                                        exitConfirmed = true; //Пользователь подтвердил выход
                                                    else if (confirmEvent.key.code == Keyboard::N)
                                                        confirmExit.close(); //Пользователь отменил выход
                                                }
                                            }
                                        }
                                        if (exitConfirmed)
                                            play.close(); //Пользователь подтвердил выход, закрываем игру
                                    }
                                }
                                if (gameOver && eventChoice.key.code == Keyboard::Escape)
                                    play.close(); //Закрываем игру, возвращаясь в главное меню
                            }

                            //Увеличиваем время и проверяем, нужно ли создавать новый шарик
                            if (!gameOver)
                            {
                                spawnTime += 0.01f; //Обновляем время
                                if (spawnTime >= spawnInterval)
                                {
                                    int randomIndex = letterRange(rng); //Случайный индекс буквы
                                    Texture& chosenTexture = balloonTextures[randomIndex]; //Случайная текстура
                                    char letter = static_cast<char>('A' + randomIndex); //Соответствующая буква

                                    Balloon newBalloon; //Создаем новый воздушный шарик
                                    newBalloon.shape.setSize(Vector2f(110, 200)); //Размер шарика
                                    newBalloon.shape.setTexture(&chosenTexture); //Случайным образом выбранная текстура
                                    newBalloon.shape.setPosition(widthRange(rng), height); //Начальное положение
                                    newBalloon.speed = speedRange(rng); //Скорость шарика
                                    newBalloon.letter = letter; //Связанная буква

                                    //Добавляем новый шарик в вектор
                                    balloons.push_back(newBalloon);

                                    //Сброс времени
                                    spawnTime = 0.0f;
                                }

                                //Определяем список клавиш, представляющих буквы от 'A' до 'Z'
                                vector<Keyboard::Key> letterKeys = {
                                    Keyboard::A, Keyboard::B, Keyboard::C, Keyboard::D,
                                    Keyboard::E, Keyboard::F, Keyboard::G, Keyboard::H,
                                    Keyboard::I, Keyboard::J, Keyboard::K, Keyboard::L,
                                    Keyboard::M, Keyboard::N, Keyboard::O, Keyboard::P,
                                    Keyboard::Q, Keyboard::R, Keyboard::S, Keyboard::T,
                                    Keyboard::U, Keyboard::V, Keyboard::W, Keyboard::X,
                                    Keyboard::Y, Keyboard::Z
                                };

                                //Обработка нажатия клавиш из заранее определенного списка
                                for (const auto& key : letterKeys)
                                {
                                    bool popped = false; //Переменная для лопания воздушного шарика
                                    if (!isReloading && Keyboard::isKeyPressed(key))
                                    {
                                        char pressedLetter = static_cast<char>('A' + (key - Keyboard::A));
                                        for (auto it = balloons.begin(); it != balloons.end(); ++it)
                                        {
                                            if (it->letter == pressedLetter)
                                            {
                                                popSound.play(); //Воспроизводим звук при лопании воздушного шарика
                                                balloons.erase(it); //Удаляем шарик, если буква совпадает
                                                popped = true;
                                                score += 10; //Начисляем 10 очков за удаление воздушного шарика
                                                break; 
                                            }
                                        }
                                        if (!popped)
                                        {
                                            isReloading = true;
                                            clock.restart();
                                        }
                                    }
                                }

                                // Обработка перезарядки
                                if (isReloading && clock.getElapsedTime().asSeconds() >= 0.5f)
                                {
                                    isReloading = false;
                                }

                                //Перемещаем шарики вверх
                                for (auto it = balloons.begin(); it != balloons.end();)
                                {
                                    it->shape.move(0, -it->speed); //Двигаем шарик вверх
                                    if (it->shape.getPosition().y < -220)
                                    {
                                        it = balloons.erase(it); //Удаляем шарик, если он вышел за пределы экрана
                                        lives--; //Уменьшаем количество жизней
                                        if (lives == 0)
                                            gameOver = true; //Завершаем игру, если жизней нет
                                    }
                                    else
                                        ++it;
                                }

                                // Обновление анимации
                                elapsed += reloadClock.restart().asSeconds();
                                if (elapsed >= frameDuration)
                                {
                                    elapsed = 0;
                                    currentFrame = (currentFrame + 1) % countFrames;
                                }

                                //Обновляем текст с текущим счётом
                                stringstream ss;
                                ss << "Score: " << score;
                                scoreText.setString(ss.str());

                                gamePurpose.close();
                                managementInformation.close();
                                authorData.close();
                                play.clear();
                                play.draw(background); //Отображаем фон
                                play.draw(scoreText); //Отображаем счет
                                if (isReloading)
                                {
                                    reloadSprite.setTextureRect(IntRect(currentFrame* distanceBetweenFrames, 0, distanceBetweenFrames, frameHeight));
                                    reloadSprite.setPosition(width / 2.2, height / 1.4);
                                    play.draw(reloadSprite);
                                }
                                //Отображение сердечек в зависимости от количества жизней
                                if (lives >= 1)
                                    play.draw(heart1); //Первое сердечко
                                if (lives >= 2)
                                    play.draw(heart2); //Второе сердечко
                                if (lives >= 3)
                                    play.draw(heart3); //Третье сердечко
                                for (const auto& balloon : balloons) {
                                    play.draw(balloon.shape); //Отображаем шарики
                                }
                                play.display(); //Обновляем окно
                            }
                            else
                            {
                                stringstream ss;
                                ss << "Your score: " << score;
                                scoreText.setString(ss.str());
                                scoreText.setCharacterSize(60);
                                scoreText.setPosition(Vector2f(width / 2.4, height / 2.0));
                                play.clear();
                                play.draw(background); //Отображаем фон
                                play.draw(gameOverText); //Отображаем сообщение об окончании игры
                                play.draw(scoreText); //Отображаем счет
                                play.display(); //Обновляем окно
                            }
                        }
                    }
                    if (menuChoice == 1)
                    {
                        Text textPurpose;
                        setText(textPurpose, font, Color::Black, 50, width / 7.0, height / 3.5);
                        string stringPurpose = "    The evil clown Pennywise decided that his time had come.\n \
He bought a lot of colorful balloons with letters and launched them into the sky.\n \
Initially, you have 3 lives, so as not to lose them, \n \
then shoot down the balloons before they leave the confines of the town of Delly.\n \
    Fortunately, a trusty air rifle will help you deal with them.\n \
    The goal of the game is to score as many points as possible, \n \
for each burst balloon you will receive 10 points. \n \
    Have a good game! \n\n\n \
    To continue, press 'Esc'";
                        textPurpose.setString(stringPurpose);

                        while (gamePurpose.isOpen())
                        {
                            Event eventChoice;
                            while (gamePurpose.pollEvent(eventChoice))
                            {
                                if (eventChoice.type == Event::Closed)
                                    gamePurpose.close();
                                if (eventChoice.type == Event::KeyPressed)
                                {
                                    if (eventChoice.key.code == Keyboard::Escape)
                                        gamePurpose.close();
                                }
                            }
                            play.close();
                            gamePurpose.clear();
                            managementInformation.close();
                            authorData.close();
                            gamePurpose.draw(background);
                            gamePurpose.draw(scoreImage);
                            gamePurpose.draw(heartsImage);
                            gamePurpose.draw(textPurpose);
                            gamePurpose.display();
                        }
                    }
                    if (menuChoice == 2)
                    {
                        Text textInfo;
                        setText(textInfo, font, Color::Black, 50, width / 9.0, height / 8.0);
                        string stringInfo = "   For a full-fledged game, there is no need to use the mouse, \n \
navigate through the menu and you can only play using the keyboard. \n \
    Pressing the letters depicted on the ball will allow it to burst, \n \
pressing an erroneous letter will lead to a miss. \n \
    It takes half a second to reload the gun, so it's better not to miss. \n \
    You can exit the game before or after its completion using the 'Esc' key \n \
    To prevent you from accidentally leaving the game, \n \
you will need to clarify before exiting whether you are going to leave the game for sure.\n\n\n\n\n\n\n \
    To continue, press 'Esc'";
                        textInfo.setString(stringInfo);
                        while (managementInformation.isOpen())
                        {
                            Event eventChoice;
                            while (managementInformation.pollEvent(eventChoice))
                            {
                                if (eventChoice.type == Event::Closed)
                                    managementInformation.close();
                                if (eventChoice.type == Event::KeyPressed)
                                {
                                    if (eventChoice.key.code == Keyboard::Escape)
                                        managementInformation.close();
                                }
                            }

                            // Обновление анимации
                            elapsed += reloadClock.restart().asSeconds();
                            if (elapsed >= 0.25f)
                            {
                                elapsed = 0;
                                currentFrame = (currentFrame + 1) % countFrames;
                            }
                            reloadSprite.setTextureRect(IntRect(currentFrame* distanceBetweenFrames, 0, distanceBetweenFrames, frameHeight));
                            reloadSprite.setPosition(width / 4.0, height / 1.6);

                            play.close();
                            gamePurpose.clear();
                            authorData.close();
                            managementInformation.clear();
                            managementInformation.draw(background);
                            managementInformation.draw(exitImage);
                            managementInformation.draw(reloadSprite);
                            managementInformation.draw(textInfo);
                            managementInformation.display();
                        }
                    }
                    if (menuChoice == 3)
                    {
                        Text textAuthor;
                        setText(textAuthor, font, Color::Black, 60, width / 2.8, height / 8.0);
                        string stringAuthor = "     Author:\n Vadim Sirko\n From BSTU 'Voenmeh'\n Group - O734B\n\n\n\n"
                      "Game development: 02.05.2024 - 13.05.2024\n\n\n\n\n To continue, press 'Esc'";
                        textAuthor.setString(stringAuthor);
                        while (authorData.isOpen())
                        {
                            Event eventChoice;
                            while (authorData.pollEvent(eventChoice))
                            {
                                if (eventChoice.type == Event::Closed)
                                    authorData.close();
                                if (eventChoice.type == Event::KeyPressed)
                                {
                                    if (eventChoice.key.code == Keyboard::Escape)
                                        authorData.close();
                                }
                            }
                            play.close();
                            gamePurpose.close();
                            managementInformation.clear();
                            authorData.clear();
                            authorData.draw(background);
                            authorData.draw(textAuthor);
                            authorData.display();
                        }
                    }
                    if (menuChoice == 4)
                        MENU.close();
                    break;
                }
            }
        }
        MENU.clear();
        MENU.draw(background);
        mainMenu.draw(MENU);
        MENU.display();
    }
    return 0;
}