/*
 * 타임-슬라이싱 실습용 main.c
 *
 *  - thread0, thread1이 같은 우선순위(7)로 등록
 *  - 스레드 안에서 명시적 yield/sleep 없이 짧게 busy-wait만 수행
 *  - CONFIG_TIMESLICING=y, CONFIG_TIMESLICE_SIZE=10이 설정된
 *    prj.conf와 함께 빌드하면 콘솔이 10 ms 간격으로 교대 출력
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

/* ──────────────────────────────────────────────
 * 스레드 설정
 * ──────────────────────────────────────────── */
#define STACK_SIZE        1024      /* 각 스레드 전용 스택(바이트)          */
#define THREAD_PRIORITY   7         /* 두 스레드 모두 같은 우선순위         */
#define BUSY_TIME_US      200       /* 0.2 ms 정도만 바쁜 연산해 로그 폭주 방지 */

/* ──────────────────────────────────────────────
 * thread0 : “작업 A” 예시
 * ──────────────────────────────────────────── */
void thread0(void)
{
	while (1) {
		printk("Hello, I am thread0\n");
		k_busy_wait(BUSY_TIME_US);   /* 아주 짧게 시간 소비 (yield 없음) */
	}
}

/* ──────────────────────────────────────────────
 * thread1 : “작업 B” 예시
 * ──────────────────────────────────────────── */
void thread1(void)
{
	while (1) {
		printk("Hello, I am thread1\n");
		k_busy_wait(BUSY_TIME_US);
	}
}

/* ──────────────────────────────────────────────
 * 정적 스레드 생성 : K_THREAD_DEFINE
 *  (엔트리, 스택, 우선순위, 옵션 0, 시작 지연 0)
 * ──────────────────────────────────────────── */
K_THREAD_DEFINE(thread0_tid, STACK_SIZE, thread0,
		NULL, NULL, NULL,
		THREAD_PRIORITY, 0, 0);

K_THREAD_DEFINE(thread1_tid, STACK_SIZE, thread1,
		NULL, NULL, NULL,
		THREAD_PRIORITY, 0, 0);

/* 별도의 main() 함수는 필요 없음.
 * 커널이 부팅되면 위 두 스레드를 바로 시작시킨다. */
