#include "HelloWorldScene.h"
#include "AppMacros.h"
USING_NS_CC;

HelloWorld::HelloWorld()
    :_targets(NULL),_projectiles(NULL)
{
}
HelloWorld::~HelloWorld()
{
    if(_targets)
    {
        _targets->release();
        _targets = NULL;
    }
    if(_projectiles)
    {
        _projectiles->release();
        _projectiles=NULL;
    }
}

CCScene* HelloWorld::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    
    // 'layer' is an autorelease object
    HelloWorld *layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !CCLayerColor::initWithColor(ccc4(255,255,255,255)))
    {
        return false;
    }
    
    CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
    CCPoint origin = CCDirector::sharedDirector()->getVisibleOrigin();

    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object
    CCMenuItemImage *pCloseItem = CCMenuItemImage::create(
                                        "CloseNormal.png",
                                        "CloseSelected.png",
                                        this,
                                        menu_selector(HelloWorld::menuCloseCallback));
    
	pCloseItem->setPosition(ccp(origin.x + visibleSize.width - pCloseItem->getContentSize().width/2 ,
                                origin.y + pCloseItem->getContentSize().height/2));

    // create menu, it's an autorelease object
    CCMenu* pMenu = CCMenu::create(pCloseItem, NULL);
    pMenu->setPosition(CCPointZero);
    this->addChild(pMenu, 1);

    /////////////////////////////
    // 3. add your codes below...

    // add a label shows "Hello World"
    // create and initialize a label

    CCSize winSize = CCDirector::sharedDirector()->getWinSize();
    CCSprite* player = CCSprite::create("Player.png");
    player->setPosition(ccp(winSize.width/2, winSize.height/15));
    //player->getContentSize().width*20
    this->addChild(player);
    
    //call game logic about every second
    this->schedule(schedule_selector(HelloWorld::gameLogic), 1.0f);
    this->schedule(schedule_selector(HelloWorld::update));
    
    
    this->setTouchEnabled(true);
    
    _targets = new CCArray;
    _projectiles = new CCArray;
    
    
    return true;
}


void HelloWorld::menuCloseCallback(CCObject* pSender)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT) || (CC_TARGET_PLATFORM == CC_PLATFORM_WP8)
	CCMessageBox("You pressed the close button. Windows Store Apps do not implement a close button.","Alert");
#else
    CCDirector::sharedDirector()->end();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
#endif
}

void HelloWorld::addTarget()
{
    CCSprite *target = CCSprite::create("Target.png");
    CCSize size = target->getContentSize();
    
    CCSize winSize = CCDirector::sharedDirector()->getWinSize();

 
    //위에서 아래로 떨어진다
    int minX = target->getContentSize().width;
    int maxX = target->getContentSize().width*40;
 
    int rangeX = maxX - minX;
    int actualX = ( rand() % rangeX ) + minX;
 
    target->setPosition(ccp(actualX,winSize.height));
    this->addChild(target);
 
 
    int minDuration = (int)2.0;
    int maxDuration = (int)4.0;
    int rangeDuration = maxDuration - minDuration;
    int actualDuration = ( rand() % rangeDuration ) + minDuration;
 
    CCFiniteTimeAction* actionMove = CCMoveTo::create((float)actualDuration, ccp(actualX,-1));
 
    CCFiniteTimeAction* actionMoveDone = CCCallFuncN::create(this,callfuncN_selector(HelloWorld::spriteMoveFinished));
 
    target->runAction(CCSequence::create(actionMove, actionMoveDone, NULL));
 
 
    /*
     //오른쪽에서 왼쪽으로 지나간다
    int minY = target->getContentSize().height/2;
    int maxY = winSize.height - target->getContentSize().height/2;
    
    int rangeY = maxY - minY;
    int actualY = ( rand() % rangeY ) + minY;
    
    target->setPosition(ccp(winSize.width + (target->getContentSize().width/2),actualY));
    
    this->addChild(target);
    
    
    int minDuration = (int)2.0;
    int maxDuration = (int)4.0;
    int rangeDuration = maxDuration - minDuration;
    int actualDuration = ( rand() % rangeDuration ) + minDuration;
    
    CCFiniteTimeAction* actionMove = CCMoveTo::create((float)actualDuration, ccp(0-target->getContentSize().width/2,actualY));

    CCFiniteTimeAction* actionMoveDone = CCCallFuncN::create(this,callfuncN_selector(HelloWorld::spriteMoveFinished));
    
    target->runAction(CCSequence::create(actionMove, actionMoveDone, NULL));
    */
    target->setTag(1);
    _targets->addObject(target);
    
}

void HelloWorld::spriteMoveFinished(CCNode* sender)
{
    CCSprite *sprite = (CCSprite *)sender;
    this -> removeChild(sprite, true);
    
    switch(sprite->getTag())
    {
        case 1:
            _targets->removeObject(sprite);
            break;
        case 2:
            _projectiles->removeObject(sprite);
            break;
        default:
            break;
            
    }
    
}


void HelloWorld::gameLogic(float dt)
{
    this->addTarget();
}

void HelloWorld::ccTouchesEnded(CCSet* touches, CCEvent* event)
{
    //Choose one of the touches to work with
    CCTouch* touch = (CCTouch*)( touches->anyObject());
    CCPoint location  = touch->getLocation();
    
    //Set up initial location of projectile
    CCSize winSize = CCDirector::sharedDirector()->getWinSize();
    CCSprite *projectile = CCSprite::create("Projectile.png");
    projectile->setPosition(ccp(250,winSize.height/15));
    
    //Determine offset of location to projectile
    int offX, offY;
    if (location.x - 250 >0)
    {
        offX = location.x - 250;
    }
    else
    {
        offX = 250 - location.x;
    }
    offY = location.y - projectile->getPosition().y;
    
    //Bail out if we are shooting down of backwards
    //if (offX <= 0) return ;
    
    //Ok to add now - we've double checkd position
    this->addChild(projectile);
    
    //Determine where we wish to shoot the projectile to
    int realX, realY;
    float ratio = (float)offY / (float)offX;
    if (location.x-250>0)
    {
        realX = 500 ;//+ (projectile->getContentSize().width/2);
        realY = (250 * ratio) + projectile->getPosition().y;
    }
    else
    {
        realX = 0 + (projectile->getContentSize().width/2);
        realY = (250 * ratio) + projectile->getPosition().y;
    }
    
    CCPoint realDest = ccp(realX,realY);
    
    /*
     기본
     projectile->setPosition(ccp(20, winSize.height/2));
     
     //Determine offset of location to projectile
     int offX, offY;
     offX = location.x - projectile->getPosition().x;
     offY = location.y - projectile->getPosition().y;
     
     //Bail out if we are shooting down of backwards
     //if (offX <= 0) return ;
     
     //Ok to add now - we've double checkd position
     this->addChild(projectile);
     
     //Determine where we wish to shoot the projectile to
     int realX, realY;
     realX = winSize.width + (projectile->getContentSize().width/2);
     float ratio = (float)offY / (float)offX;
     realY = ( realX * ratio) + projectile->getPosition().y;
     CCPoint realDest = ccp(realX,realY);
     

     */
    //Determine the length of how far we're shooting
    int offRealX = realX - projectile->getPosition().x;
    int offRealY = realY - projectile->getPosition().y;
    float length = sqrtf((offRealX*offRealX) + (offRealY*offRealY));
    float velocity = 480/1; //480pixels / 1sec
    float RealMoveDuration = length/velocity;
    
    //Move projectile to actual endpoint
    CCFiniteTimeAction * moved = CCMoveTo::create(RealMoveDuration, realDest);
    CCFiniteTimeAction * finished = CCCallFuncN::create(this,callfuncN_selector(HelloWorld::spriteMoveFinished));
    
    projectile->runAction( CCSequence::create(moved, finished,NULL));
    
    projectile->setTag(2);
    _projectiles->addObject(projectile);
    
    
}

void HelloWorld::update(float dt)
{
    CCArray *projectilesToDelete = new CCArray;
    CCObject* it = NULL;
    CCObject* jt = NULL;
    
    CCARRAY_FOREACH(_projectiles, it)
    {
        CCSprite *projectile = static_cast<CCSprite*>(it);
        CCRect projectileRect = CCRectMake(
        projectile->getPosition().x - (projectile->getContentSize().width/2),
        projectile->getPosition().y -(projectile->getContentSize().height/2),
        projectile->getContentSize().width,
        projectile->getContentSize().height);
        CCArray* targetsToDelete = new CCArray;
        
        CCARRAY_FOREACH(_targets, jt)
        {
            CCSprite *target = static_cast<CCSprite*>(jt);
            CCRect targetRect = CCRectMake(
            target->getPosition().x - (target->getContentSize().width/2),
            target->getPosition().y - (target->getContentSize().height/2),
            target->getContentSize().width,
            target->getContentSize().height);
            
            if(projectileRect.intersectsRect(targetRect))
               {
                   targetsToDelete->addObject(target);
               }
        }
        
        CCARRAY_FOREACH(targetsToDelete, jt)
        {
            CCSprite* target = static_cast<CCSprite*>(jt);
            _targets->removeObject(target);
            this->removeChild(target, true);
            
        }
        
        if(targetsToDelete->count() >0)
        {
            projectilesToDelete->addObject(projectile);
        }
        targetsToDelete->release();
    }
    CCARRAY_FOREACH(projectilesToDelete,it)
    {
        CCSprite* projectile = static_cast<CCSprite*>(it);
        _projectiles->removeObject(projectile);
        this->removeChild(projectile,true);
    }
    projectilesToDelete->release();
}

