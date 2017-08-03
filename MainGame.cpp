#include "ui/CocosGUI.h"
#include "GameOverScene.hpp"
#include "MainGame.hpp"
#include "AudioEngine.h"
#include "Definitions.h"
#include "2d/CCRenderTexture.h"
#include "TextureBlur.h"


//using namespace ui;
USING_NS_CC;


Scene* MainGame::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = MainGame::create();
    
    // add layer as a child to scene
    scene->addChild(layer);
    
    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool MainGame::init()
{
    //////////////////////////////
    // 1. super init first
    if (!Layer::init())
    {
        return false;
    }
    
    auto visibleSize = Director::getInstance()->getVisibleSize();                                //! Для расстановки изображений
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    
    cocos2d::experimental::AudioEngine::play2d("BirdSingingSFX.mp3", true, 0.85f);              // Включаем звук птиц
    

    background = Sprite::create("bg.png");																//! Создаём фон
    meat = MenuItemImage::create("meat.png","meat.png", CC_CALLBACK_1(MainGame::Points,this));          //! Это должен быть кусок мяса ;
    man = Sprite::create("1.4.png");
    plate = Sprite::create("plate.png");
    table = Sprite::create("table.png");
    
    

    
    man->setSpriteFrame(SpriteFrame::create("1.4.png", Rect(0, 0, 100, 300)));
    
    MenRed = SpriteFrame::create("men2.png", Rect(0, 0, 300, 800));
    
    
    menu = Menu::create(meat, NULL);                      //? Переделать потом - пока так!!!!!!!!!!!!!!!!!!!
    menu->setEnabled(false);								  //!!!!!!!!!!!!!!!!
    
    
    score = 0;   //! Устанавливаем счет
    
    __String *tempScore = __String::createWithFormat("TAP TO PLAY!");       //! Создаём надпись "Tap To Play!"
    scoreLabel = Label::createWithTTF(tempScore->getCString(), "fonts/Archive.otf", visibleSize.height * SCORE_FONT_SIZE);
    scoreLabel->setColor(Color3B::BLACK);
    
    
    //!!!!! Создаём анимацию надписи "Tap To Play!"
    auto TapLabelFadeOut = FadeOut::create(2.0f);
    auto TapLabelFadeIn = FadeIn::create(1.5f);
    auto DelayTapToPlay = DelayTime::create(2.0f);
    auto TapLabelSequence = Sequence::create(TapLabelFadeOut, DelayTapToPlay,TapLabelFadeIn, nullptr);    //! Создаём сиквенцию с нашими действиями, "анимацию"
    
    repeatTapLabel = RepeatForever::create(TapLabelSequence);     //! Зацикливаем "анимацию" надписи
    TapLabelAnimation();  //! запускаем анимацию (Вызываем функцию 1 раз)
    //!!!!! Создаём анимацию надписи "Tap To Play!"
    
    
    
    //!!!!! Создаем TouchEvent - при нажатии по экрану - игра запускается
    contactListener = EventListenerTouchOneByOne::create();             //?????
    contactListener->setSwallowTouches(true);                           //??????
    contactListener->setEnabled(true);                                  //???????
    contactListener->onTouchBegan = CC_CALLBACK_1(MainGame::onCantactBegin, this);   //! Должен отключить возможность нажатия после запуска функции "onCantactBegin"
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(contactListener, this);           //????????
    //!!!!! Создаем TouchEvent - при нажатии по экрану - игра запускается
    
    
    scoreLabel->setPosition(Point(visibleSize.width / 2 + origin.x, visibleSize.height * 0.75f + origin.y));
    background->setPosition(Point(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
    menu->setPosition(Point(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y - 90));
    table->setPosition(Point(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y - 120));
    man->setPosition(Point(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
    plate->setScale(0.65f);
    plate->setPosition(Point(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y - 100));
    
    
    
    meat->setScale(0.75f);
    table->setScale(0.75f);
    background->setScale(0.75f);
    
    
    
    MainGame::addChild(background,1);
    MainGame::addChild(man, 3);
    MainGame::addChild(table, 4);
    MainGame::addChild(scoreLabel, 5);
    MainGame::addChild(plate, 5);
    MainGame::addChild(menu, 6);

    MainGame::scheduleUpdate();                 //???????????
    
    return true;
}
//! Разoрбраться с -> ::  .

void MainGame::TapLabelAnimation()            //! Запускаем зацикленную анимацию надписи "Tap To Play!"
{
    scoreLabel->runAction(repeatTapLabel);
}


bool MainGame::onCantactBegin(cocos2d::Touch *touch)      //! Запускаем игру после нажатия по экрану
{
    stopAction(repeatTapLabel);								//! Выключаем зацикленную анимацию надписи "Tap to play!"
    scoreLabel->setOpacity(255);							  // Устанавливаем прозрачность счетчика очков на 255
    scoreLabel->setString("0");								  // Устанавливаем счет на 0
    
    contactListener->setEnabled(false);					//! Запрещаем принимать "все" нажатия по экрану
    menu->setEnabled(true);       // Разрешаем тапать на мясо
    
    MainGame::PauseButtonCreate();				// Создание кнопки паузы
    MainGame::schedule(schedule_selector(MainGame::MenWatch), 6);				// Запускает врага
    
    return true;
}


void MainGame::PauseButtonCreate()        //! Создаём и добавляем на экран кнопку паузы
{
    
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    
    auto button = cocos2d::ui::Button::create("GUI_button_pause_orange.png", "GUI_button_pause_orange-dark.png", "GUI_button_pause_orange-dark.png");  ////
    button->setPosition(Point((visibleSize.width / 2 + origin.x) * 1.25f, (visibleSize.height / 2 + origin.y) * 1.75f));                                //  Создаём кнопку паузы и задаём ей параметры
    button->setScale(0.75f);																														   ////
    
    
    auto &buttonpause = button;			// Создаём ссылку для лямбда функции
    
    
    button->addTouchEventListener([&, buttonpause](Ref* sender, cocos2d::ui::Widget::TouchEventType type)                //! Функционал кнопки паузы
                                  {
                                      switch (type)
                                      {
                                          case cocos2d::ui::Widget::TouchEventType::BEGAN:
                                              
                                              break;
                                          case cocos2d::ui::Widget::TouchEventType::MOVED:
                                              break;
                                          case cocos2d::ui::Widget::TouchEventType::ENDED:
                                              
                                              menu->setEnabled(false);		// Выкчлючаем возможность есть мясо
                                              
                                              man->pause();
                                              
                                              
                                              if (sfxOn)
                                              {
                                                  cocos2d::experimental::AudioEngine::play2d("Tap.mp3", false, 1.0f);
                                              }
                                              
                                              scoreLabel->setOpacity(0);        // Делаем счет невидимым
                                              
                                              MainGame::PauseGame();            // Размываем задний фон и ???????ставим игру на паузу
                                              MainGame::PauseMenuCreate();      // Создаём меню паузы
                                              //? Тут ошибка - создаю одно и тоже дохера раз похоже      - но так как функция локальная всё ок
                                              //? Игра должна ставить на паузу

                                              MainGame::removeChild(buttonpause, true);   // Удаляем с экрана кнопку паузы  ( только с экрана в пямити всё осталось )
                                              break;
                                              
                                          case cocos2d::ui::Widget::TouchEventType::CANCELED:
                                              break;
                                              
                                          default:
                                              break;
                                      }
                                      
                                  });
    
     MainGame::addChild(button, 12);		// Добавляем кнопку паузы на экран
}

void MainGame::PauseMenuCreate()					//! Создание меню паузы
{
    auto visibleSize = Director::getInstance()->getVisibleSize();				//? Мб добавить их в хедер раз они везде юзаются????
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    
    
    // Создаём все кнопки
    auto resume = cocos2d::ui::Button::create("GUI_button_play_orange.png", "GUI_button_play_orange-dark.png", "GUI_button_play_orange.png");
    auto sfx = cocos2d::ui::CheckBox::create("SFXON.png",
                                             "SFXTAP.png",
                                             "SFXOFF.png",
                                             "GUI_button_play_orange.png",
                                             "GUI_button_play_orange.png");
    
    
    // Создаём ссылки для лямбдо функции
    auto &sfxlink = sfx;
    auto &resumelink = resume;
    
    // Устанавливаем позиции и размер
    sfx->setPosition(Point((visibleSize.width / 2 + origin.x) * 1.25f, (visibleSize.height / 2 + origin.y) * 1.75f));
    sfx->setScale(1.00);
    
    resume->setPosition(Point(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y ));
    resume->setScale(0.75);
    
    // Добавляем на экран все кнопки
    this->addChild(sfx, 12);
    this->addChild(resume, 12);
    
    
    
    resume->addTouchEventListener([&, resumelink, sfxlink](Ref* sender, cocos2d::ui::Widget::TouchEventType type)		//! Функционал кнопки "продолжить"
                                  {
                                      switch (type)
                                      {
                                          case cocos2d::ui::Widget::TouchEventType::BEGAN:
                                              log("TAP BEGAN BUTTON PLAY");
                                              break;
                                          case cocos2d::ui::Widget::TouchEventType::MOVED:
                                              break;
                                          case cocos2d::ui::Widget::TouchEventType::ENDED:
                                              log("TAP ENDED BUTTON PLAY");
                                              MainGame::removeChild(resumelink, true);
                                              MainGame::removeChild(sfxlink, true);
                                           //   MainGame::removeChild(pausebg, true);           ОШИБКА БАГ ТЫ ПИДОР ПЕТЯ
                                              //? Продолжить игру здесь надо
                                              menu->setEnabled(true);						// Включаем возможность "есть" мясо
                                              scoreLabel->setOpacity(255);
                                              PauseButtonCreate();					//  Создаём кнопку паузы
                                              man->resume();
                                              MainGame::PauseGame();
                                              break;
                                          case cocos2d::ui::Widget::TouchEventType::CANCELED:
                                              break;
                                          default:
                                              break;
                                      }
                                      
                                  });
    
    
    
    
    if (sfxOn == false)				///Графически настраиваем вид кнопки
    {
        sfx->setSelected(true);
    }
    
    sfx->addTouchEventListener([&, sfxlink](Ref* sender, cocos2d::ui::Widget::TouchEventType type) //! Функционал регулятора музыки
                               {
                                   switch (type)
                                   {
                                       case ui::Widget::TouchEventType::BEGAN:
                                           break;
                                       case ui::Widget::TouchEventType::ENDED:
                                           
                                           if (sfxOn)
                                           {
                                               sfxOn = false;
                                               cocos2d::experimental::AudioEngine::pauseAll();					//? Тут чет все криво
                                               
                                           }
                                           else
                                           {
                                               sfxOn = true;
                                               cocos2d::experimental::AudioEngine::resumeAll();				//? Тут тоже не очень
                                           }
                                           
                                           
                                           break;
                                       default:
                                           break;
                                   }
                               });
    
    
}

void MainGame::PauseGame()   //? Размытие по Гауссу и пауза ( но мб это объеденю в одну функцию )
{
    if (gamerunning)
    {
        gamerunning = false;
        
        auto visibleSize = Director::getInstance()->getVisibleSize();                                //! Для расстановки изображений
        Vec2 origin = Director::getInstance()->getVisibleOrigin();

        Size screenSize = Director::getInstance()->getWinSize();
        
        
        auto rendert = RenderTexture::create(screenSize.width, screenSize.height);
        rendert->retain();
        rendert->begin();
        background->visit();
        man->visit();
        table->visit();
        plate->visit();
        menu->visit();
        rendert->end();
        
        TextureBlur::initShader(screenSize / 1.0f);
        auto texture = TextureBlur::create(rendert->getSprite()->getTexture() , 1.1f, 1);
        auto bluredbg = Sprite::createWithTexture(texture);
        bluredbg->getTexture()->setAntiAliasTexParameters();
        bluredbg->setPosition(Point(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
        bluredbg->setScale(1.0f);
        bluredbg->setFlippedY(true);
        pausebg = bluredbg;
        addChild(pausebg, 7);
        
        
        
    }
    
    else
    {
        gamerunning = true;
        removeChild(pausebg, true);
    }
  
}

void MainGame::Points(cocos2d::Ref *sender) //! Должен считать результат, уменьшать мясо, и проверять не проиграл ли игрок
{
    auto meatscale = ScaleTo::create(0.5, meat->getScale() * 0.85);			// Анимация уменьшения мяса
    
    if (man->getSpriteFrame() != MenRed)		// Проверяет является ли текущий кадр, кадром при котором игрок проигрывает, если нет - добовляет очков
    {
        if (meat->getScale() > 0.4)
        {
            meat->runAction(meatscale);
        }
        
        else
        {
            meat->setScale(1.0);
        }
        
        score++;        /// Увеличиваем кол-во очков на 1
        __String *tempScore = __String::createWithFormat("%i", score);  // Обновляем счет
        scoreLabel->setString(tempScore->getCString());     //??????
        
    }
    
    else    //// Запускаем Game Over Screen
    {
        cocos2d::experimental::AudioEngine::stopAll();      // Отключаем всю музыку
        auto scene = GameOver::createScene();               // Создаём и добавляем Game Over Screen
        Director::getInstance()->replaceScene(TransitionFade::create(TRANSITION_TIME, scene));
    }
  
}

void MainGame::MenWatch(float dt)  //! Должен генерировать поведение противника
{
    Vector<SpriteFrame*> turningAnimation;
    
    MenRed = SpriteFrame::create("1.1.png", Rect(0, 0, 100, 300));		// Положение мужика , когда он смотрит за игроком
    
    turningAnimation.pushBack(SpriteFrame::create("1.4.png", Rect(0, 0, 105, 300)));
    turningAnimation.pushBack(SpriteFrame::create("1.3.png", Rect(0, 0, 105, 300)));
    turningAnimation.pushBack(MenRed);
    Animation* animation = Animation::createWithSpriteFrames(turningAnimation, 1.5f);
    Animate* animateTurning = Animate::create(animation);
    
    
    Vector<SpriteFrame*> fastTurning;
    
    fastTurning.pushBack(MenRed);
    fastTurning.pushBack(SpriteFrame::create("1.2.png", Rect(0, 0, 105, 300)));
    Animation* animation2 = Animation::createWithSpriteFrames(fastTurning, 1.5f);
    Animate* animateFTurning = Animate::create(animation2);
    
    
    Vector<SpriteFrame*> Relax;
    Relax.pushBack(SpriteFrame::create("1.4.png", Rect(0, 0, 105, 300)));
    Relax.pushBack(SpriteFrame::create("1.3.png", Rect(0, 0, 105, 300)));
    Animation* animation3 = Animation::createWithSpriteFrames(Relax, 1.5f);
    Animate* animateRelax = Animate::create(animation3);
    
    int z = random(0,2);
    switch (z)
    {
        case 0:
            log("Turning");
            man->runAction(animateTurning);
            break;
        case 1:
            man->runAction(animateRelax);
            log("Relax");
            break;
        case 2:
            man->runAction(animateFTurning);
            log("FTunring");
            break;
    }
    log("%i", z);
    
}






 

