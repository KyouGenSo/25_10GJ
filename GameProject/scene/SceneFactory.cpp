#include "SceneFactory.h"
#include "TitleScene.h"
#include "GameScene.h"
#include "ClearScene.h"
#include "OverScene.h"
#include "TutorialScene.h"

BaseScene* SceneFactory::CreateScene(const std::string& sceneName)
{
	BaseScene* newScene = nullptr;

	if (sceneName == "title") {
		newScene = new TitleScene();
	} 
	else if (sceneName == "game") {
		newScene = new GameScene();
	}
    else if (sceneName == "tutorial") {
        newScene = new TutorialScene();
    }
    else if (sceneName == "clear") {
        newScene = new ClearScene();
    }
    else if (sceneName == "over") {
        newScene = new OverScene();
    }

	return newScene;
}
