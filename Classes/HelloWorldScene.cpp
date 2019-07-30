/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 
 http://www.cocos2d-x.org
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;

PhysicsWorld* HelloWorld::physicsWorld = nullptr;

Scene* HelloWorld::createScene()
{
    auto scene = Scene::createWithPhysics();
    scene->getPhysicsWorld()->setGravity(Vec2(0, -980));
    
    HelloWorld::physicsWorld = scene->getPhysicsWorld();
    
    auto layer = HelloWorld::create();
    scene->addChild(layer);
    
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    
    auto background = Sprite::create("background.png");
    background->setPosition(visibleSize/2);
    this->addChild(background);
    
    auto laut = Sprite::create("water.png");
    laut->setPosition(Vec2(640, 50));
    this->addChild(laut);
    
    auto tanah = Sprite::create("ground.png");
    tanah->setPosition(Vec2(640, 70));
    tanah->setName("tanah");
    this->addChild(tanah);
    
    auto tanahBody = PhysicsBody::createBox(tanah->getContentSize());
    tanahBody->setDynamic(false);
    tanahBody->setContactTestBitmask(0xFFFFFFFF);
    tanah->setPhysicsBody(tanahBody);
    
    auto data = FileUtils::getInstance()->getValueVectorFromFile("level1.plist");
    
    for (int i = 0; i < (int) data.size(); i++) {
        auto item = data[i].asValueMap();
        
        if (item["type"].asString() == "circle") {
            int posx = item["position"].asValueMap()["x"].asInt();
            int posy = item["position"].asValueMap()["y"].asInt();
            int angle = item["angle"].asInt();
            
            auto circle = Sprite::create("circle.png");
            circle->setPosition(Vec2(posx, posy));
            
            auto body = PhysicsBody::createCircle(35);
            circle->setPhysicsBody(body);
            circle->setRotation(angle);
            this->addChild(circle);
        }
        else if (item["type"].asString() == "box1") {
            int posx = item["position"].asValueMap()["x"].asInt();
            int posy = item["position"].asValueMap()["y"].asInt();
            int angle = item["angle"].asInt();
            
            auto box = Sprite::create("box1.png");
            box->setPosition(Vec2(posx, posy));
            
            auto body = PhysicsBody::createBox(box->getContentSize());
            box->setPhysicsBody(body);
            box->setRotation(angle);
            this->addChild(box);
        }
        else if (item["type"].asString() == "box2") {
            int posx = item["position"].asValueMap()["x"].asInt();
            int posy = item["position"].asValueMap()["y"].asInt();
            int angle = item["angle"].asInt();
            
            auto box = Sprite::create("box2.png");
            box->setPosition(Vec2(posx, posy));
            
            auto body = PhysicsBody::createBox(box->getContentSize());
            box->setPhysicsBody(body);
            box->setRotation(angle);
            this->addChild(box);
        }
        else if (item["type"].asString() == "alien" && alien == nullptr) {
            int posx = item["position"].asValueMap()["x"].asInt();
            int posy = item["position"].asValueMap()["y"].asInt();
            
            alien = Sprite::create("alien.png");
            alien->setPosition(Vec2(posx, posy));
            alien->setName("alien");
            
            auto body = PhysicsBody::createBox(alien->getContentSize());
            body->setContactTestBitmask(0xFFFFFFFF);
            alien->setPhysicsBody(body);
            this->addChild(alien);
        }
    }
    
    this->scheduleUpdate();
    
    auto listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = [=](Touch* touch, Event* event){
        Vec2 touchLocation = touch->getLocation();
        
        auto shape = physicsWorld->getShape(touchLocation);
        
        if (shape) {
            auto node = shape->getBody()->getNode();
            
            if (node->getName() != "alien" && node->getName() != "tanah") {
                node->removeFromParent();
            }
        }
        
        return true;
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    
    auto listener2 = EventListenerKeyboard::create();
    listener2->onKeyReleased = [=] (EventKeyboard::KeyCode keyCode, Event * event) {
        if (keyCode == EventKeyboard::KeyCode::KEY_ENTER) {
            Director::getInstance()->replaceScene(HelloWorld::createScene());
        }
        cocos2d::log("key pressed");
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener2, this);
    
    auto contactListener = EventListenerPhysicsContact::create();
    contactListener->onContactBegin = CC_CALLBACK_1(HelloWorld::onContactBegin, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(contactListener, this);
    
    return true;
}

void HelloWorld::update(float dt) {
    auto visibleSize = Director::getInstance()->getVisibleSize();
    
    
    if (alien) {
        if (alien->getPositionX() < 0 || alien->getPositionX() > visibleSize.width ||
            alien->getPositionY() < 0) {
            
            //kalah
            MessageBox("YOU LOSE!", "Try Again");
            Director::getInstance()->replaceScene(HelloWorld::createScene());
        }
    }
    
    if (startCounting) {
        time -= dt;
        if (time <= 0) {
            MessageBox("YOU WIN!", "Congratulation");
            startCounting = false;
            time = 3;
        }
    }
}

bool HelloWorld::onContactBegin(PhysicsContact& contact) {
    auto nodeA = contact.getShapeA()->getBody()->getNode();
    auto nodeB = contact.getShapeB()->getBody()->getNode();
    
    if (nodeA && nodeB) {
        if ((nodeA->getName() == "alien" && nodeB->getName() == "tanah") ||
            (nodeB->getName() == "alien" && nodeA->getName() == "tanah")) {
            //menang
            if (!startCounting) startCounting = true;
        }
    }
    
    return true;
}
