#include "ui\CocosGUI.h"
#include "GameOverScene.h"
#include "MainGame.h"
#include "AudioEngine.h"
#include "Definitions.h"


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

	auto background = Sprite::create("bg2.png");																//! Добавляем фон
	meat = MenuItemImage::create("Meat.png","Meat.png", CC_CALLBACK_1(MainGame::Points,this));   //! Это должен быть кусок мяса ; 
	auto plate = Sprite::create("plate.png");
	auto table = Sprite::create("table2.1.png");
	

	men2 = Sprite::create("menturning.png");
	men2->setPosition(Point(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
	
	MainGame::addChild(men2, 3);

	men2->setSpriteFrame(SpriteFrame::create("menturning.png", Rect(0, 0, 300, 800)));

	menu = Menu::create(meat, NULL);                      //? Переделать потом - пока так
	menu->setEnabled(false);								  //
	MainGame::addChild(menu, 6);
 
	score = 0;   //! Устанавливаем счет

	__String *tempScore = __String::createWithFormat("TAP TO PLAY!");   //! Создаём надпись "Tap To Play!"

	scoreLabel = Label::createWithTTF(tempScore->getCString(), "Archive.otf", visibleSize.height * SCORE_FONT_SIZE);  
	
	MenRed = SpriteFrame::create("men2.png", Rect(0, 0, 300, 800));

	cocos2d::experimental::AudioEngine::play2d("BirdSingingSFX.mp3", true, 1.0f);
	


	//! Создаём анимацию надписи "Tap To Play!"
	auto TapLabelFadeOut = FadeOut::create(2.0f);
	auto TapLabelFadeIn = FadeIn::create(1.5f);
	auto DelayTapToPlay = DelayTime::create(2.0f);
	auto TapLabelSequence = Sequence::create(TapLabelFadeOut, DelayTapToPlay,TapLabelFadeIn, nullptr);    //! Создаём сиквенцию с нашими действиями, "анимацию" 
	
	repeatTapLabel = RepeatForever::create(TapLabelSequence);     //! Зацикливаем "анимацию" надписи

	TapLabelAnimation();  //! запускаем анимацию (Вызываем функцию 1 раз)
	//! Создаём анимацию надписи "Tap To Play!"


	
	//! Создаем TouchEvent - при нажатии по экрану - игра запускается 
	contactListener = EventListenerTouchOneByOne::create();
	contactListener->setSwallowTouches(true);
	contactListener->setEnabled(true);
	contactListener->onTouchBegan = CC_CALLBACK_1(MainGame::onCantactBegin, this);   //! Должен отключить возможность нажатия после запуска функции "onCantactBegin"
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(contactListener, this);
	//! Создаем TouchEvent - при нажатии по экрану - игра запускается 


	auto &scoreL = scoreLabel;

	scoreLabel->setColor(Color3B::BLACK);
	scoreLabel->setPosition(Point(visibleSize.width / 2 + origin.x, visibleSize.height * 0.75f + origin.y));

	MainGame::addChild(scoreLabel, 5);

	background->setPosition(Vec2(384, 512));
	menu->setPosition(Point(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y - 160));
	
	table->setPosition(Point(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y + 150));
	plate->setScale(0.75f);
	plate->setPosition(Point(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y - 180));

	MainGame::addChild(plate, 5);
	MainGame::addChild(background,1);
	MainGame::addChild(table, 4);
	MainGame::scheduleUpdate();	

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
	menu->setEnabled(true);       //? Включаем стол

	MainGame::PauseButtonCreate();				// Создание кнопки паузы
	MainGame::schedule(schedule_selector(MainGame::MenWatch), 6);				// Запускает врага 
	return true;
}


void MainGame::PauseButtonCreate()        //! Создаём и добавляем на экран кнопку паузы
{

	auto visibleSize = Director::getInstance()->getVisibleSize();        //? Мб добавить их в хедер раз они везде юзаются????
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	auto button = cocos2d::ui::Button::create("GUI_button_pause_orange.png", "GUI_button_pause_orange-dark.png", "GUI_button_pause_orange-dark.png");  ////
	button->setPosition(Point(visibleSize.width / 2 + origin.x + 300, visibleSize.height / 2 + origin.y + 400));									  //  Создаём кнопку паузы и задаём ей параметры
	button->setScale(0.75);																															 ////


	auto &buttonpause = button;			// Создаём ссылку для лямбда функции

	MainGame::addChild(button, 6);		// Добавляем кнопку паузы на экран 

	button->addTouchEventListener([&, buttonpause](Ref* sender, cocos2d::ui::Widget::TouchEventType type)                //! Функционал кнопки паузы
	{
		switch (type)
		{
		case cocos2d::ui::Widget::TouchEventType::BEGAN:
			log("Tap Start");
			
			menu->setEnabled(false);		// Выкчлючаем возможность есть мясо
			
			break;
		case cocos2d::ui::Widget::TouchEventType::MOVED:
			break;
		case cocos2d::ui::Widget::TouchEventType::ENDED:
			if (sfxOn)
			{
				cocos2d::experimental::AudioEngine::play2d("Tap.mp3", false, 1.0f);
			}

			scoreLabel->setOpacity(0);

			log("TAP ENDED BUTTON PAUSE");
			MainGame::removeChild(buttonpause, true);   // Удаляем с экрана кнопку паузы  ( только с экрана в пямити всё осталось )  
			MainGame::PauseMenuCreate();    //? Тут ошибка - создаю одно и тоже дохера раз похоже      - но так как функция локальная всё ок
			                            //? Игра должна ставить на паузу
			MainGame::setpausebg();  ///Декоративная чушь чисто

			break;

		case cocos2d::ui::Widget::TouchEventType::CANCELED:
			break;

		default:
			break;
		}

	});

}

void MainGame::CreditMenuCreate()
{
	auto visibleSize = Director::getInstance()->getVisibleSize();        //? Мб добавить их в хедер раз они везде юзаются????
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	auto credit = cocos2d::ui::Button::create("t-1.png", "t-1.png", "t-1.png");  ////
	credit->setPosition(Point(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));									  //  Создаём кнопку меню титров
	credit->setScale(0.75);																															 ////


	auto &creditlink = credit;			// Создаём ссылку для лямбда функции

	MainGame::addChild(credit, 6);		// Добавляем кнопку паузы на экран 

	credit->addTouchEventListener([&, creditlink](Ref* sender, cocos2d::ui::Widget::TouchEventType type)                //! Функционал меню титров
	{
		switch (type)
		{
		case cocos2d::ui::Widget::TouchEventType::BEGAN:
			log("Credit Tap Statred");
			break;
		case cocos2d::ui::Widget::TouchEventType::MOVED:
			break;

		case cocos2d::ui::Widget::TouchEventType::ENDED:
			log("Credit Tap Ended");
			MainGame::removeChild(creditlink, true);   // Удаляем с экрана кнопку паузы  ( только с экрана в пямити всё осталось )  
			MainGame::PauseMenuCreate();    //? Тут ошибка - создаю одно и тоже дохера раз похоже      - но так как функция локальная всё ок
											//? Игра должна ставить на паузу

			break;

		case cocos2d::ui::Widget::TouchEventType::CANCELED:
			break;

		default:
			break;
		}

	});

	


}


void MainGame::PauseMenuCreate()					//! Создание меню паузы
{	
	auto visibleSize = Director::getInstance()->getVisibleSize();				//? Мб добавить их в хедер раз они везде юзаются????
	Vec2 origin = Director::getInstance()->getVisibleOrigin();


	// Создаём все кнопки
	auto resume = cocos2d::ui::Button::create("GUI_button_play_orange.png", "GUI_button_play_orange-dark.png", "GUI_button_play_orange.png");
	auto info = cocos2d::ui::Button::create("GUI_button_play_orange.png", "GUI_button_play_orange-dark.png", "GUI_button_play_orange.png");
	auto sfx = cocos2d::ui::CheckBox::create("SFXON.png",
		"SFXTAP.png",
		"SFXOFF.png",
		"GUI_button_play_orange.png",
		"GUI_button_play_orange.png");


	// Создаём ссылки для лямбдо функции
	auto &sfxlink = sfx;
	auto &infolink = info;
	auto &resumelink = resume;

	// Устанавливаем позиции и размер
	sfx->setPosition(Point(visibleSize.width / 2 + origin.x + 280, visibleSize.height / 2 + origin.y + 420));
	sfx->setScale(1.00);

	info->setPosition(Point(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y + 200));
	info->setScale(0.75);

	resume->setPosition(Point(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y + 400));
	resume->setScale(0.75);

	// Добавляем на экран все кнопки
	this->addChild(sfx, 12);
	this->addChild(info, 12);
	this->addChild(resume, 12);



	resume->addTouchEventListener([&, resumelink, infolink, sfxlink](Ref* sender, cocos2d::ui::Widget::TouchEventType type)		//! Функционал кнопки "продолжить"
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
			MainGame::removeChild(infolink, true);
			MainGame::removeChild(sfxlink, true);
			MainGame::removeChild(pausebg, true);
															//? Продолжить игру здесь надо
			menu->setEnabled(true);						// Включаем возможность "есть" мясо
			scoreLabel->setOpacity(255);
			PauseButtonCreate();					//  Создаём кнопку паузы
			break;
		case cocos2d::ui::Widget::TouchEventType::CANCELED:
			break;
		default:
			break;
		}

	});

	
	info->addTouchEventListener([&, resumelink, infolink, sfxlink](Ref* sender, cocos2d::ui::Widget::TouchEventType type)		//! Функционал кнопки "титры"
	{
		switch (type)
		{
		case cocos2d::ui::Widget::TouchEventType::BEGAN:
			log("TAP BEGAN CREDITS");
			break;
		case cocos2d::ui::Widget::TouchEventType::MOVED:
			break;
		case cocos2d::ui::Widget::TouchEventType::ENDED:
			log("TAP ENDED BUTTON");
			MainGame::removeChild(resumelink, true);
			MainGame::removeChild(infolink, true);
			MainGame::removeChild(sfxlink, true);
			MainGame::removeChild(pausebg, true);
			CreditMenuCreate();

			
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
			log("AUDIO CLICED");
		
			if (sfxlink->isSelected())
			{
				////CocosDenshion::SimpleAudioEngine::getInstance()->setEffectsVolume(0.0f);
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

void MainGame::PauseGame(cocos2d::Ref *sender)   //? Размытие по Гауссу и пауза ( но мб это объеденю в одну функцию )
{
	if (gameplaying)
	{
		Director::getInstance()->pause();
		gameplaying = false;
	}
	else
	{
		Director::getInstance()->resume();
		gameplaying = true;
	}
}

void MainGame::Points(cocos2d::Ref *sender) //! Должен считать результат, уменьшать мясо, и проверять не проиграл ли игрок
{
	auto t = ScaleTo::create(0.5, meat->getScale() * 0.75);			// Анимация уменьшения мяса

	if (men2->getSpriteFrame() != MenRed)		// Проверяет является ли текущий кадр, кадром при котором игрок проигрывает, если нет - добовляет очков
	{
		if (meat->getScale() > 0.2)
		{
			meat->runAction(t);
		}
		else
		{
			meat->setScale(1.0);
		}
		score++;

		__String *tempScore = __String::createWithFormat("%i", score);

		scoreLabel->setString(tempScore->getCString());

	}

	else
	{
	
		auto scene = GameOver::createScene();
		Director::getInstance()->replaceScene(TransitionFade::create(TRANSITION_TIME, scene));
	}
}

void MainGame::MenWatch(float dt)  //! Должен генерировать поведение противника
{
	Vector<SpriteFrame*> turningAnimation;

	MenRed = SpriteFrame::create("men2.png", Rect(0, 0, 300, 800));		// Положение мужика , когда он смотрит за игроком

	turningAnimation.pushBack(SpriteFrame::create("men1.png", Rect(0, 0, 300, 800)));
	turningAnimation.pushBack(SpriteFrame::create("menturned.png", Rect(0, 0, 300, 800)));
	turningAnimation.pushBack(MenRed);
	Animation* animation = Animation::createWithSpriteFrames(turningAnimation, 1.5f);
	Animate* animateTurning = Animate::create(animation);

	
	Vector<SpriteFrame*> fastTurning;

	fastTurning.pushBack(MenRed);
	fastTurning.pushBack(SpriteFrame::create("menturned.png", Rect(0, 0, 300, 800)));
	Animation* animation2 = Animation::createWithSpriteFrames(fastTurning, 1.5f);
	Animate* animateFTurning = Animate::create(animation2);

	
	Vector<SpriteFrame*> Relax;
	Relax.pushBack(SpriteFrame::create("menturning.png", Rect(0, 0, 300, 800)));
	Relax.pushBack(SpriteFrame::create("men1.png", Rect(0, 0, 300, 800)));
	Animation* animation3 = Animation::createWithSpriteFrames(Relax, 1.5f);
	Animate* animateRelax = Animate::create(animation3);

	int z = random(0,2);
switch (z)
	{
	case 0:
		log("Turning");
		men2->runAction(animateTurning);
		break;
	case 1:
		men2->runAction(animateRelax);
		log("Relax");
		break;
	case 2:
		men2->runAction(animateFTurning);
		log("FTunring");
	break;
	}

}

void MainGame::setpausebg()			// Декорация 
{
	auto visibleSize = Director::getInstance()->getVisibleSize();	
	Vec2 origin = Director::getInstance()->getVisibleOrigin();


	pausebg = Sprite::create("PauseBG.png");

	pausebg->setPosition(Point(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y + 20));
	pausebg->setOpacity(155);

	this->addChild(pausebg, 8);


}
