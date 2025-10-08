#include <iostream>
#include <libusb.h>
#include <stdio.h>

using namespace std;

void printdev(libusb_device *dev) {
    libusb_device_descriptor desc; // дескриптор устройства
    libusb_config_descriptor *config; // дескриптор конфигурации объекта
    libusb_device_handle *handle;
    
    int r = libusb_get_device_descriptor(dev, &desc);
    if (r < 0) {
        fprintf(stderr, "Ошибка: дескриптор устройства не получен, код: %d.\n", r);
        return;
    }

    r = libusb_open(dev, &handle);
    if (r < 0) {
        fprintf(stderr, "Ошибка: не удалось открыть устройство, код: %d.\n", r);
        libusb_free_config_descriptor(config);
        return;
    }

    // получить конфигурацию устройства
    libusb_get_config_descriptor(dev, 0, &config);
    printf("%.2d %.2d %.4d %.4d %.3d\n",
        (int)desc.bNumConfigurations,
        (int)desc.bDeviceClass,
        desc.idVendor,
        desc.idProduct,
        (int)config->bNumInterfaces
    );

    for(int i = 0; i < (int)config->bNumInterfaces; i++) {
        const libusb_interface *inter = &config->interface[i];
        printf("| | | | | %.2d %.2d\n", inter->num_altsetting, (int)desc.bDeviceClass);
        for(int j = 0; j < inter->num_altsetting; j++) {
            const libusb_interface_descriptor *interdesc = &inter->altsetting[j];
            printf("| | | | | | | %.2d %.2d\n", (int)interdesc->bInterfaceNumber, (int)interdesc->bNumEndpoints);
            for(int k = 0; k < (int)interdesc->bNumEndpoints; k++) {
                const libusb_endpoint_descriptor *epdesc = &interdesc->endpoint[k];
                printf("| | | | | | | | | %.2d %.9d\n", (int)epdesc->bDescriptorType, (int)epdesc->bEndpointAddress);
            }
        }
    }

    // Получение серийного номера
    if (desc.iSerialNumber) {
        unsigned char serial_number[256];
        r = libusb_get_string_descriptor_ascii(handle, desc.iSerialNumber, serial_number, sizeof(serial_number));
        if (r <= 0) {
            fprintf(stderr, "Ошибка: не удалось получить серийный номер устройства, код: %d.\n", r);
        } else {
            printf("| | | | | | | | | | | %s\n", serial_number);
        }
    }

     // Получение строк производителя и продукта
    if (desc.iManufacturer) {
        unsigned char manufacturer[256];
        r = libusb_get_string_descriptor_ascii(handle, desc.iManufacturer, manufacturer, sizeof(manufacturer));
        if (r <= 0) {
            fprintf(stderr, "Ошибка: не удалось получить имя производителя устройства, код: %d.\n", r);
        } else {
            printf("%s\n", manufacturer);
        }
    }

    if (desc.iProduct) {
        unsigned char product[256];
        r = libusb_get_string_descriptor_ascii(handle, desc.iProduct, product, sizeof(product));
            std::cout << "Product: " << product << std::endl;
        if (r <= 0) {
            fprintf(stderr, "Ошибка: не удалось получить имя устройства, код: %d.", r);
        } else {
            printf("%s\n", product); 
        }
    }

    libusb_close(handle);
    libusb_free_config_descriptor(config);
}

int main() {
    // используется для получения списка устройств
    libusb_device **devs; // указатель на указатель на устройство,
    libusb_context *ctx = NULL; // контекст сессии libusb

    // инициализировать библиотеку libusb, открыть сессию работы с libusb
    int r = libusb_init(&ctx);
    if(r < 0) {
        fprintf(stderr, "Ошибка: инициализация не выполнена, код: %d.\n", r);
        return 1;
    }

    // задать уровень подробности отладочных сообщений
    libusb_set_option(ctx, LIBUSB_OPTION_LOG_LEVEL, LIBUSB_LOG_LEVEL_INFO);

    // получить список всех найденных USB-устройств
    // cnt - число найденных USB-устройств
    int cnt = libusb_get_device_list(ctx, &devs);
    if(cnt < 0) {
        fprintf(stderr, "Ошибка: список USB устройств не получен.\n");
        return 1;
    }

    printf("найдено устройств: %zd\n", cnt);
    printf("===========================================================\n");
    printf("* количество возможных конфигураций\n");
    printf("| * класс устройства\n");
    printf("| | * идентификатор производителя\n");
    printf("| | | * идентификатор устройства\n");
    printf("| | | | * количество интерфейсов\n");
    printf("| | | | | * количество альтернативных настроек\n");
    printf("| | | | | | * класс устройства\n");
    printf("| | | | | | | * номер интерфейса\n");
    printf("| | | | | | | | * количество конечных точек\n");
    printf("| | | | | | | | | * тип дескриптора\n");
    printf("| | | | | | | | | | * адрес конечной точки\n");
    printf("| | | | | | | | | | | * серийный номер\n");
    printf("+--+--+----+----+---+--+--+--+--+--+----------------------\n");

    for(int i = 0; i < cnt; i++) { // цикл перебора всех устройств
        printdev(devs[i]); // печать параметров устройства
    }
    printf("===========================================================\n");

    // освободить память, выделенную функцией получения списка устройств
    libusb_free_device_list(devs, 1);
    // завершить работу с библиотекой libusb
    // закрыть сессию работы с libusb
    libusb_exit(ctx);

    return 0;
}