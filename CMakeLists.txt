cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Inicializa o SDK do Raspberry Pi Pico
include(pico_sdk_import.cmake)

project(EmbarcaTarefa C CXX ASM)

# Inicializa o SDK do Raspberry Pi Pico
pico_sdk_init()

# Adiciona o executável
add_executable(EmbarcaTarefa main.c)

# Configura o nome e versão do programa
pico_set_program_name(EmbarcaTarefa "EmbarcaTarefa")
pico_set_program_version(EmbarcaTarefa "0.1")

# Desabilita saída por UART/USB
pico_enable_stdio_uart(EmbarcaTarefa 0)
pico_enable_stdio_usb(EmbarcaTarefa 0)

# Adiciona a biblioteca padrão ao build
target_link_libraries(EmbarcaTarefa pico_stdlib)

# Inclui os diretórios de cabeçalhos necessários para as bibliotecas
target_include_directories(EmbarcaTarefa PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${PICO_SDK_PATH}/src/rp2_common/hardware_pio/include
    ${PICO_SDK_PATH}/src/rp2_common/hardware_clocks/include
    ${PICO_SDK_PATH}/src/rp2_common/hardware_adc/include
    ${PICO_SDK_PATH}/src/rp2_common/pico_runtime/include
)

# Linka as bibliotecas necessárias para o hardware
target_link_libraries(EmbarcaTarefa 
    hardware_pio
    hardware_clocks
    hardware_adc
)

# Gera os arquivos de saída extra
pico_add_extra_outputs(EmbarcaTarefa)
