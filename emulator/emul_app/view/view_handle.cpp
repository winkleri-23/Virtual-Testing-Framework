#include "view_handle.hpp"

AppView* viewhandle::view = nullptr;

inline void viewhandle::init_default_view()
{
    view = new ConsoleView();
    register_msg_source(viewhandle::APP_TAG, Logger::getMsgQueue());
}

inline void viewhandle::enable_console_color_view()
{
    AppView* newView = new ColoredConsoleView();

    if (view != nullptr) {
        newView->setViewConf(viewhandle::view->getViewConf());
        delete view;
    }

    viewhandle::view = newView;
}

inline void viewhandle::register_msg_source(std::string_view tag, MessageQueue* msgSrc)
{
    view->registerSource(tag, msgSrc);
}

void viewhandle::unregister_msg_source(std::string_view tag)
{
    view->unregisterSource(tag);
}

bool viewhandle::set_msg_lvl(const char* lvl)
{
    auto query = LogType::parent(std::string_view(lvl));

    if (!query.has_value())
        return false;

    view->setMsgLvl(query.value());

    return true;
}

bool viewhandle::set_msg_lvl(LogType::type lvl)
{
    view->setMsgLvl(lvl);
    return true;
}

inline void viewhandle::run()
{
    view->run();
}

void viewhandle::stop()
{
    view->stop();
}
