/*
 * main.c - Exercise 1‑5 : UART로 1~3 입력 시 LED 3개 토글
 * 모든 주석은 한국어로 쉽게 작성
 */

#include <zephyr/kernel.h>          /* Zephyr의 핵심 API */
#include <zephyr/drivers/uart.h>    /* UART 드라이버 */
#include <zephyr/drivers/gpio.h>    /* GPIO 드라이버 */
#include <zephyr/sys/printk.h>      /* printk() 출력용 */

/* ========= 설정값 ========= */
#define RX_BUF_SIZE     10          /* 수신 버퍼 크기 (바이트) */
#define RX_TIMEOUT_US   100         /* UART가 조용하면 몇 µs 뒤 RX 이벤트 발생 */

/* ========= 전역 변수 ========= */
static const struct device *uart_dev;          /* UART 디바이스 핸들 */
static uint8_t rx_buf[RX_BUF_SIZE];            /* 수신 버퍼 */

/* LED 3개 핀 정보 (Device Tree alias 사용) */
static const struct gpio_dt_spec led0 = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);
static const struct gpio_dt_spec led1 = GPIO_DT_SPEC_GET(DT_ALIAS(led1), gpios);
static const struct gpio_dt_spec led2 = GPIO_DT_SPEC_GET(DT_ALIAS(led2), gpios);

/* ========= 함수 선언 ========= */
static void uart_event_cb(const struct device *dev,
                          struct uart_event *evt,
                          void *user_data);

/* ========= 메인 함수 ========= */
int main(void)
{
    /* 1) UART 디바이스 얻기 */
    uart_dev = DEVICE_DT_GET(DT_NODELABEL(uart0)); /* uart0는 USB CDC에 매핑 */
    if (!device_is_ready(uart_dev)) {
        printk("UART 디바이스 준비 실패\n");
        return 0;
    }

    /* 2) LED 핀을 출력으로 설정 */
    gpio_pin_configure_dt(&led0, GPIO_OUTPUT_INACTIVE); /* 시작 시 꺼짐(0) */
    gpio_pin_configure_dt(&led1, GPIO_OUTPUT_INACTIVE);
    gpio_pin_configure_dt(&led2, GPIO_OUTPUT_INACTIVE);

    /* 3) UART 이벤트 콜백 등록 */
    uart_callback_set(uart_dev, uart_event_cb, NULL);

    /* 4) 안내 메시지 송신 (논블로킹) */
    static const char intro_msg[] =
        "\r\nnRF Connect SDK UART 예제\r\n"
        "키보드 1~3을 누르면 LED 1~3이 토글됩니다.\r\n";
    uart_tx(uart_dev, intro_msg, sizeof(intro_msg), SYS_FOREVER_US);

    /* 5) 수신 시작 */
    uart_rx_enable(uart_dev, rx_buf, sizeof(rx_buf), RX_TIMEOUT_US);

    /* 6) main 스레드는 할 일이 없으므로 대기 */
    while (true) {
        k_sleep(K_FOREVER); /* 절전 대기 */
    }
}

/* ========= UART 콜백 ========= */
static void uart_event_cb(const struct device *dev,
                          struct uart_event *evt,
                          void *user_data)
{
    switch (evt->type) {
    case UART_RX_RDY: {
        /* 새 데이터가 도착함 */
        uint8_t c = evt->data.rx.buf[evt->data.rx.offset];
        switch (c) {
        case '1':
            gpio_pin_toggle_dt(&led0);  /* LED1 토글 */
            break;
        case '2':
            gpio_pin_toggle_dt(&led1);  /* LED2 토글 */
            break;
        case '3':
            gpio_pin_toggle_dt(&led2);  /* LED3 토글 */
            break;
        default:
            /* 1~3 이외 문자는 무시 */
            break;
        }
        break;
    }
    case UART_RX_DISABLED:
        /* 버퍼가 가득 찼거나 타임아웃 발생 → 다시 수신 시작 */
        uart_rx_enable(dev, rx_buf, sizeof(rx_buf), RX_TIMEOUT_US);
        break;
    case UART_TX_DONE:
        /* 안내 메시지 전송 완료 (필요 시 처리) */
        break;
    default:
        /* 다른 이벤트는 이번 예제에서 사용 안 함 */
        break;
    }
}
