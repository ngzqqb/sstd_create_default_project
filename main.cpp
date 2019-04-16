#include <thread>
#include <atomic>
#include <string>
#include <chrono>
#include <string_view>
#include <sstream>

#include <mutex>
#include <condition_variable>

using namespace std::chrono_literals;
using namespace std::string_literals;
using namespace std::string_view_literals;

#if __has_include(<filesystem>)
#include <filesystem>
namespace fs = std::filesystem;

inline static const fs::path & get_file_path(const fs::path & arg) {
    return arg;
}

#else
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;

inline static std::string get_file_path(const fs::path & arg) {
    return fs::canonical(arg).string();
}

#endif

#include <list>
#include <vector>
#include <exception>

#include <iostream>
#include <fstream>

#include <regex>

class ReadStream : public std::ifstream {
    using super = std::ifstream;
public:
    inline ReadStream(const fs::path & path) :
        super(get_file_path(path), std::ios::binary) {
        this->sync_with_stdio(false);
    }
    ReadStream(const ReadStream &) = delete;
    ReadStream(ReadStream &&) = delete;
};

class WriteStream : public std::ofstream {
    using super = std::ofstream;
public:
    inline WriteStream(const fs::path & path) :
        super(get_file_path(path), std::ios::binary) {
        this->sync_with_stdio(false);
    }
    WriteStream(const WriteStream &) = delete;
    WriteStream(WriteStream &&) = delete;
};

template<  typename ... Args >
std::string print(Args && ... args) {
    std::string varAns;
    (varAns += ... += std::forward<Args>(args));
    return std::move(varAns);
}

inline constexpr std::string_view getBom() {
    return "\xEF\xBB\xBF"sv;
}

class Main {
public:
    fs::path outdir;
    std::string projectName;
    std::string projectModuleName;
    inline void construct() {
        projectModuleName = projectName + "_module"s;
    }
    inline void createProject() {
        fs::create_directories(outdir / projectName);
        {
            WriteStream varWrite{ outdir / projectName / (projectName + ".pro"s) };
            varWrite << print("# ", projectName, "/", projectName, ".pro", '\n',
                '\n',
                "TEMPLATE = app"sv, '\n',
                '\n',
                "CONFIG(debug,debug|release){"sv, '\n',
                "    TARGET = "sv, projectName, "_debug"sv, '\n',
                "}else{", '\n',
                "    TARGET = "sv, projectName, '\n',
                '}', '\n',
                '\n',
                "SOURCES += $$PWD/main.cpp"sv, '\n',
                '\n',
                "include($$PWD/../../sstd_library/sstd_library.pri)"sv, '\n',
                "include($$PWD/../../sstd_qt_qml_quick_library/sstd_qt_qml_quick_library.pri)"sv, '\n',
                "include($$PWD/../../sstd_library/add_vc_debug_console.pri)"sv, '\n',
                '\n',
                "DESTDIR = $${SSTD_LIBRARY_OUTPUT_PATH}"sv, '\n',
                '\n',
                "CONFIG(debug,debug|release){"sv, '\n',
                "    DEFINES += CURRENT_DEBUG_PATH=\\\"$$PWD\\\""sv, '\n',
                "}else{"sv, "\n"sv,
                "    QMAKE_POST_LINK += $$escape_expand(\\n\\t)$${DESTDIR}/sstd_copy_qml $${PWD}/appqml $${DESTDIR}/appqml release"sv, "\n"sv,
                "    export(QMAKE_POST_LINK)"sv, "\n"sv,
                "}"sv, "\n"sv,
                '\n',
                "QMLSOURCES += $$PWD/appqml/"sv, projectName, "/main.qml"sv, '\n',
                '\n',
                "lupdate_only{"sv, "\n"sv,
                "    SOURCES += $$QMLSOURCES", "\n"sv,
                '}', '\n',
                '\n',
                "DISTFILES += $$QMLSOURCES"sv, '\n',
                '\n',
                u8R"(#/*endl_input_of_latex_for_clanguage_lick*/")", '\n');
        }
        {
            WriteStream varWrite{ outdir / projectName / "main.cpp"sv };
            varWrite << print(getBom(), "/* "sv, projectName, "/"sv, "main.cpp"sv, " */"sv, '\n',
                '\n',
                "int main(int argc, char ** argv) {"sv, '\n',
                '\n',
                "    sstd::QtApplication varApplication{ argc,argv };"sv, '\n',
                '\n',
                "    QQmlApplicationEngine varEngine;"sv, '\n',
                "    {"sv, '\n',
                u8R"(        /*获得Qml文件绝对路径*/)"sv, '\n',
                "        auto varFullFileName = sstd::autoLocalPath<QUrl>("sv, '\n',
                u8R"(            QStringLiteral("appqml/)"sv, projectName, u8R"(/main.qml"));)", '\n',
                u8R"(        /*加载Qml文件*/)", '\n',
                u8R"(        varEngine.load(varFullFileName);)"sv, '\n',
                u8R"(        /*检查并报错*/)", '\n',
                u8R"(        if (varEngine.rootObjects().empty()) {)"sv, '\n',
                u8R"(            return -1;)"sv, '\n',
                u8R"(        })", '\n',
                u8R"(    })", '\n',
                u8R"(    return varApplication.exec();)", '\n',
                '\n',
                u8R"(})", '\n',
                '\n',
                u8R"(/*endl_input_of_latex_for_clanguage_lick*/)", '\n');
        }
        fs::create_directories(outdir / projectName / "appqml"sv / projectName);
        {
            WriteStream varWrite{ outdir / projectName / "appqml"sv / projectName / "main.qml"sv };
            varWrite << print(getBom(), "/* "sv, "appqml"sv, "/", projectName, "/"sv, "main.qml"sv, " */"sv, '\n',
                "import QtQuick 2.9", '\n',
                "import QtQuick.Controls 2.5", '\n',
                '\n',
                u8R"(/*begin:import*/)"sv, '\n',
                "import theqml_the_debug."sv, projectModuleName, " 1.0"sv, '\n',
                u8R"(/*end:import*/)", '\n',
                '\n',
                u8R"(ApplicationWindow {)"sv, '\n',
                '\n',
                "    width: 360 ;", '\n',
                "    height: 64 ;", '\n',
                "    visible: true ;", '\n',
                '\n',
                '}', '\n',
                u8R"(/*endl_input_of_latex_for_clanguage_lick*/)", '\n',
                u8R"(/*begin:debug*/)", '\n',
                u8R"(/*end:debug*/)", '\n');

        }
    }
    inline void createProjectModule() {
        fs::create_directories(outdir / projectModuleName);
        fs::create_directories(outdir / projectModuleName / "theqml_the_debug"s / projectModuleName);
        {
            WriteStream varWrite{ outdir / projectModuleName / (projectModuleName + ".pro"s) };
            varWrite << print("# "sv, projectModuleName, "/"sv, projectModuleName, ".pro"sv, '\n',
                '\n',
                "TEMPLATE = lib"sv, '\n',
                "CONFIG += plugin", '\n',
                "TARGET = $$qtLibraryTarget("sv, projectModuleName, ")"sv, "\n"sv,
                '\n',
                "SOURCES += $$PWD/the_moudle.cpp"sv, '\n',
                "HEADERS += $$PWD/the_moudle.hpp"sv, '\n',
                '\n',
                "SOURCES += $$PWD/Empty.cpp"sv, '\n',
                "HEADERS += $$PWD/Empty.hpp"sv, '\n',
                '\n',
                "include($$PWD/../../sstd_library/sstd_library.pri)", '\n',
                "include($$PWD/../../sstd_qt_qml_quick_library/sstd_qt_qml_quick_library.pri)", '\n',
                '\n',
                "mkpath($${SSTD_LIBRARY_OUTPUT_PATH}/theqml_the_debug/"sv, projectModuleName, ")"sv, '\n',
                "CONFIG(debug,debug|release) {", '\n',
                "    DESTDIR = $${SSTD_LIBRARY_OUTPUT_PATH}/theqml_the_debug/", projectModuleName, '\n',
                "    QMAKE_POST_LINK += $$escape_expand(\\n\\t)$${SSTD_LIBRARY_OUTPUT_PATH}/sstd_copy_qml $${PWD}/theqml_the_debug $${SSTD_LIBRARY_OUTPUT_PATH}/theqml_the_debug debug", '\n',
                "}else{", '\n',
                "    DESTDIR = $${SSTD_LIBRARY_OUTPUT_PATH}/theqml/", projectModuleName, '\n',
                "    QMAKE_POST_LINK += $$escape_expand(\\n\\t)$${SSTD_LIBRARY_OUTPUT_PATH}/sstd_copy_qml $${PWD}/theqml_the_debug $${SSTD_LIBRARY_OUTPUT_PATH}/theqml release", '\n',
                "    QMAKE_POST_LINK += $$escape_expand(\\n\\t)$$[QT_INSTALL_BINS]/qmlplugindump -notrelocatable theqml.",
                projectModuleName,
                " 1.0 $${SSTD_LIBRARY_OUTPUT_PATH} > $${SSTD_LIBRARY_OUTPUT_PATH}/theqml_the_debug/",
                projectModuleName,
                "/plugins.qmltypes", "\n"sv,
                '}', '\n',
                "export(QMAKE_POST_LINK)", '\n',
                '\n',
                "DISTFILES += $$PWD/theqml_the_debug/"sv, projectModuleName, "/qmldir"sv, '\n',
                '\n',
                u8R"(/*endl_input_of_latex_for_clanguage_lick*/)", '\n');
        }
        {
            WriteStream varWrite{ outdir / projectModuleName / "theqml_the_debug"s / projectModuleName / "qmldir"s };
            varWrite << print("module theqml_the_debug.", projectModuleName, '\n',
                "plugin ", projectModuleName, '\n',
                /*only for debug*/"typeinfo plugins.qmltypes"sv, '\n',
                /*only for static library,
                in the book ,
                only use dynamic library,
                so never need!*/"#classname sstd::TheMoudle"sv, '\n');
        }
        {
            WriteStream varWrite{ outdir / projectModuleName / "the_moudle.hpp"sv };
            varWrite << print(getBom(), "/* "sv, projectModuleName, "/"sv, "the_moudle.hpp"sv, " */"sv, '\n',
                "#pragma once", '\n',
                '\n',
                "#include <sstd_qt_qml_quick_library.hpp>", '\n',
                '\n',
                "namespace sstd {", '\n',
                '\n',
                "    class TheMoudle : public QQmlExtensionPlugin {", '\n',
                "        Q_OBJECT", '\n',
                "    private:", '\n',
                "        Q_PLUGIN_METADATA(IID QQmlExtensionInterface_iid)", '\n',
                "    public:", '\n',
                "        void registerTypes(const char *uri) override;", '\n',
                "    private:", '\n',
                "        sstd_class(TheMoudle);", '\n',
                "    };", '\n',
                '\n',
                "}/*namespace sstd*/", '\n',
                '\n',
                u8R"(/*endl_input_of_latex_for_clanguage_lick*/)", '\n');
        }
        {
            WriteStream varWrite{ outdir / projectModuleName / "the_moudle.cpp"sv };
            varWrite << print(getBom(), "/* "sv, projectModuleName, "/"sv, "the_moudle.cpp"sv, " */"sv, '\n',
                '\n',
                u8R"(#include "the_moudle.hpp")"sv, '\n',
                u8R"(#include "Empty.hpp")"sv, '\n',
                '\n',
                "void sstd::TheMoudle::registerTypes(const char * argURI) {"sv, '\n',
                u8R"(    qmlRegisterType<Empty>(argURI, 1, 0, "Empty");)", '\n',
                "}"sv, '\n',
                '\n',
                u8R"(/*endl_input_of_latex_for_clanguage_lick*/)", '\n');
        }
        {
            WriteStream varWrite{ outdir / projectModuleName / "Empty.hpp"sv };
            varWrite << print(getBom(), "/* "sv, projectModuleName, "/"sv, "Empty.hpp"sv, " */"sv, '\n',
                "#pragma once", '\n',
                '\n',
                "#include <sstd_qt_qml_quick_library.hpp>", '\n',
                '\n',
                "namespace sstd {", '\n',
                '\n',
                "    class Empty : public QObject {"sv, '\n',
                "    private:"sv, '\n',
                "        Q_OBJECT"sv, '\n',
                "    public:", '\n',
                "        Empty();", '\n',
                "    private:", '\n',
                "        sstd_class(Empty);", '\n',
                "    }"sv, '\n',
                '\n',
                "}"sv,
                '\n',
                u8R"(/*endl_input_of_latex_for_clanguage_lick*/)", '\n');
        }
        {
            WriteStream varWrite{ outdir / projectModuleName / "Empty.cpp"sv };
            varWrite << print(getBom(), "/* "sv, projectModuleName, "/"sv, "Empty.cpp"sv, " */"sv, '\n',
                '\n',
                u8R"(#include "Empty.hpp")"sv, '\n',
                '\n',
                "sstd::Empty::Empty() {"sv, '\n',
                "}"sv, '\n',
                '\n',
                u8R"(/*endl_input_of_latex_for_clanguage_lick*/)", '\n');
        }
    }
};

/*
0:application path
1:out dir
2:project name
*/
int main(int argc, char ** argv) try {

    if (argc < 2) {
        return -1;
    }

    auto varMain = new Main;
    if (argc > 2) {
        varMain->outdir = argv[1];
        varMain->projectName = argv[2];
    } else {
        varMain->projectName = argv[1];
        varMain->outdir = fs::path(argv[0]).parent_path();
    }
    varMain->construct();
    varMain->createProject();
    varMain->createProjectModule();

} catch (const std::exception & arg) {
    std::cout << arg.what() << std::endl;
}

