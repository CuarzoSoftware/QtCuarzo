project(
    'qtcuarzo',
    'c', 'cpp',
    version : '0.1.0',
    meson_version : '>= 0.60.0',
    default_options : [
        'warning_level=2',
        'cpp_std=c++20',
    ]
)

# ------------------------------ Dependencies ------------------------------

qt6 = import('qt6')

# private_headers pulls in the QPA (qpa/qplatform*.h) headers this plugin is built on.
qt6_dep = dependency('qt6',
    modules : ['Core', 'Gui'],
    private_headers : true)

wayland_client_dep = dependency('wayland-client')
cz_core_dep        = dependency('cz-core')
cz_heaven_dep      = dependency('cz-heaven-client')

# ------------------------- lvr-private-handle protocol -------------------------

wayland_scanner = find_program('wayland-scanner')
protocol_xml    = 'src/protocols/lvr-private-handle.xml'

ph_client_header = custom_target('lvr-private-handle-client-header',
    input   : protocol_xml,
    output  : 'lvr-private-handle-client.h',
    command : [wayland_scanner, 'client-header', '@INPUT@', '@OUTPUT@'])

ph_private_code = custom_target('lvr-private-handle-private-code',
    input   : protocol_xml,
    output  : 'lvr-private-handle.c',
    command : [wayland_scanner, 'private-code', '@INPUT@', '@OUTPUT@'])

# --------------------------------- Sources ---------------------------------

moc_headers = [
    'src/Plugin.h',
    'src/PlatformTheme.h',
    'src/HNMenuBar.h',
    'src/HNMenu.h',
    'src/HNMenuItem.h',
]

# moc needs the source dir on its include path to resolve the Q_PLUGIN_METADATA FILE json.
moc_files = qt6.compile_moc(
    headers   : moc_headers,
    dependencies : qt6_dep,
    extra_args : ['-I' + meson.current_source_dir() / 'src'])

sources = [
    'src/main.cpp',
    'src/PlatformTheme.cpp',
    'src/HNMenuBar.cpp',
    'src/HNMenu.cpp',
    'src/HNMenuItem.cpp',
    moc_files,
    ph_client_header,
    ph_private_code,
]

# --------------------------------- Install ---------------------------------

qmake = find_program('qmake6', 'qmake', required : false)

if qmake.found()
    plugins_dir = run_command(qmake, '-query', 'QT_INSTALL_PLUGINS', check : true).stdout().strip()
else
    plugins_dir = get_option('libdir') / 'qt6' / 'plugins'
endif

shared_module('cuarzo',
    sources,
    include_directories : include_directories('src'),
    dependencies : [qt6_dep, wayland_client_dep, cz_core_dep, cz_heaven_dep],
    install : true,
    install_dir : plugins_dir / 'platformthemes')
