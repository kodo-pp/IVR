#include <graphics/graphics.hpp>
#include <irrlicht.h>
#include <vector>
#include <string>

#include <unistd.h>

namespace graphics {
    irr::IrrlichtDevice * irrDevice = nullptr;
    irr::video::IVideoDriver * irrVideoDriver = nullptr;
    irr::scene::ISceneManager * irrSceneManager = nullptr;
    irr::gui::IGUIEnvironment * irrGuiEnvironment = nullptr;
}

bool initializeGraphics(std::vector <std::string> * args) {
    if (args == nullptr) {
        return false;
    }

    // TODO: возможно, вынести код инициализации Irrlicht в отдельныю функцию
    graphics::irrDevice = irr::createDevice(
        irr::video::EDT_OPENGL, // Драйвер для рендеринга (здесь OpenGL) (см. http://irrlicht.sourceforge.net/docu/example001.html)
        irr::core::dimension2d <irr::u32> (800, 600), // Размеры окна (не в полноэкранном режиме)
        32, // Глубина цвета
        false, // Полноэкранный режим
        false, // stencil buffer (не очень понятно, что это. COMBAK)
        false, // Вертикальная синхронизация
        0); // Объект-обработчик событий (здесь его нет)
    if (!graphics::irrDevice) {
        // TODO: добавить fallback-настройки
        return false;
    }

    graphics::irrDevice->setWindowCaption(L"Test window"); // COMBAK

    // Получаем необходимые указатели на объекты
    graphics::irrVideoDriver = graphics::irrDevice->getVideoDriver();
    if (!graphics::irrVideoDriver) {
        return false;
    }

    graphics::irrSceneManager = graphics::irrDevice->getSceneManager();
    if (!graphics::irrSceneManager) {
        return false;
    }

    graphics::irrGuiEnvironment = graphics::irrDevice->getGUIEnvironment();
    if (!graphics::irrSceneManager) {
        return false;
    }

    // ***********************************************************************
    // Тест, потом убрать

    graphics::irrGuiEnvironment->addStaticText(L"Hello world!", // Сам текст
        irr::core::rect <irr::s32> (10, 10, 260, 260), // Положение текста на экране (прямоугольная область)
        true); // Неясно, что это

    graphics::irrVideoDriver->beginScene(true, // Неясно, что это
        true, // Неясно, что это
        irr::video::SColor(255, 100, 101, 140)); // Какой-то цвет, возможно, цвет фона (ARGB)
    graphics::irrGuiEnvironment->drawAll();
    graphics::irrVideoDriver->endScene();
    sleep(3);
    graphics::irrDevice->drop();

    // Конец теста
    // ***********************************************************************
    return true;
}
