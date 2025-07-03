/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>

// 10분(600000ms) 동안 대기
#define SLEEP_TIME_MS   (10 * 60 * 1000)

// 버튼 sw0 노드 가져오기
#define SW0_NODE        DT_ALIAS(sw0)
static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET(SW0_NODE, gpios);

// LED led0 노드 가져오기
#define LED0_NODE       DT_ALIAS(led0)
static const struct gpio_dt_spec led    = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

// 버튼 인터럽트가 발생했을 때 실행될 함수
static void 버튼_인터럽트_cb(const struct device *dev,
                             struct gpio_callback *cb,
                             uint32_t pins)
{
    // 버튼 상태 읽어서 LED에 반영
    bool 상태 = gpio_pin_get_dt(&button);
    gpio_pin_set_dt(&led, 상태);
}

// 콜백 정보를 담을 구조체
static struct gpio_callback 버튼_cb_data;

int main(void)
{
    int ret;

    // LED 장치 준비 확인
    if (!device_is_ready(led.port)) {
        return -1;
    }
    // 버튼 장치 준비 확인
    if (!device_is_ready(button.port)) {
        return -1;
    }

    // LED 핀을 출력으로 설정 (초기 켜짐)
    ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
    if (ret < 0) {
        return -1;
    }

    // 버튼 핀을 입력으로 설정
    ret = gpio_pin_configure_dt(&button, GPIO_INPUT);
    if (ret < 0) {
        return -1;
    }

    // 버튼 핀에 상승/하강 엣지 인터럽트 설정
    ret = gpio_pin_interrupt_configure_dt(&button, GPIO_INT_EDGE_BOTH);
    if (ret < 0) {
        return -1;
    }

    // 콜백 구조체 초기화
    gpio_init_callback(&버튼_cb_data, 버튼_인터럽트_cb, BIT(button.pin));
    // 인터럽트에 콜백 등록
    gpio_add_callback(button.port, &버튼_cb_data);

    // 메인 루프: 인터럽트만 기다림
    while (1) {
        k_msleep(SLEEP_TIME_MS);
    }
}
