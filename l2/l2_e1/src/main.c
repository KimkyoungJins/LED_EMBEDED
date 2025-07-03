/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>

// STEP 7 - 100ms로 변경
#define SLEEP_TIME_MS 100  

// STEP 3.1 - 버튼 노드 가져오기
#define SW0_NODE   DT_ALIAS(sw0)  

// LED0 노드 가져오기
#define LED0_NODE  DT_ALIAS(led0)  

// LED 정보 가져오기 (포트, 핀, 설정)
static const struct gpio_dt_spec led    = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
// 버튼 정보 가져오기 (포트, 핀, 설정)
static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET(SW0_NODE, gpios);

int main(void)
{
    int ret;

    // LED 디바이스 준비 확인
    if (!device_is_ready(led.port)) {
        return -1;
    }

    // 버튼 디바이스 준비 확인
    if (!device_is_ready(button.port)) {
        return -1;
    }

    // STEP 4: LED 핀을 출력으로 설정, 초기 상태는 켜짐
    ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
    if (ret < 0) {
        return -1;
    }

    // STEP 5: 버튼 핀을 입력으로 설정
    ret = gpio_pin_configure_dt(&button, GPIO_INPUT);
    if (ret < 0) {
        return -1;
    }

    while (1) {
        // STEP 6.1: 버튼 상태 읽기 (눌리면 1, 안 눌리면 0)
        bool val = gpio_pin_get_dt(&button);

        // STEP 6.2: 버튼 상태에 따라 LED 켜고 끄기
        gpio_pin_set_dt(&led, val);

        // 100ms 대기
        k_msleep(SLEEP_TIME_MS);
    }
}
