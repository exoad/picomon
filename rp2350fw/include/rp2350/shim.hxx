// C++ wrappers for the SDK calls a Kira extern cannot make: an enum argument (gpio_function_t), a byte passed by
// pointer, and a UART or I2C instance chosen by number. Each one calls the SDK function it names and nothing more.
#pragma once

#include <cstdint>

#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "hardware/uart.h"

#if defined(PICO_CYW43_SUPPORTED)
#include "pico/cyw43_arch.h"
#endif

#ifndef PICOMON_BOARD
#error "PICOMON_BOARD must be set to the board's index in rp2350.board.PROFILES (platformio.ini)"
#endif

namespace rp2350::shim
{
  inline constexpr std::int32_t FUNC_SIO = GPIO_FUNC_SIO;
  inline constexpr std::int32_t FUNC_UART = GPIO_FUNC_UART;
  inline constexpr std::int32_t FUNC_I2C = GPIO_FUNC_I2C;
  inline constexpr std::int32_t FUNC_PWM = GPIO_FUNC_PWM;

#if defined(PICO_CYW43_SUPPORTED)
#ifndef CYW43_WL_GPIO_LED_PIN
// Every CYW43 board puts the LED on chip GPIO 0 (boards/pico2_w.h); the arduino-pico variant does not include that header.
#define CYW43_WL_GPIO_LED_PIN 0
#endif

  inline bool cyw43Init()
  {
      return cyw43_arch_init() == 0;
  }

  inline void cyw43WriteLed(bool high)
  {
      cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, high);
  }
#else
  // Without the wireless chip's driver linked, these are never reached: the program only calls them on a board that has the chip.
  inline bool cyw43Init()
  {
      return false;
  }

  inline void cyw43WriteLed(bool)
  {
  }
#endif

  inline void setFunction(std::uint32_t gpio, std::int32_t fn)
  {
      gpio_set_function(gpio, static_cast<gpio_function_t>(fn));
  }

  inline uart_inst_t *uartAt(std::int32_t n)
  {
      return n == 0 ? uart0 : uart1;
  }

  inline std::uint32_t uartBegin(std::int32_t n, std::uint32_t baud)
  {
      return uart_init(uartAt(n), baud);
  }

  inline void uartPutByte(std::int32_t n, std::uint8_t b)
  {
      uart_putc_raw(uartAt(n), static_cast<char>(b));
  }

  inline bool uartReadable(std::int32_t n)
  {
      return uart_is_readable(uartAt(n));
  }

  inline std::uint8_t uartGetByte(std::int32_t n)
  {
      return static_cast<std::uint8_t>(uart_getc(uartAt(n)));
  }

  inline i2c_inst_t *i2cAt(std::int32_t n)
  {
      return n == 0 ? i2c0 : i2c1;
  }

  inline std::uint32_t i2cBegin(std::int32_t n, std::uint32_t baud)
  {
      return i2c_init(i2cAt(n), baud);
  }

  inline std::int32_t i2cWriteByte(std::int32_t n, std::int32_t address, std::uint8_t b, bool nostop)
  {
      return i2c_write_blocking(i2cAt(n), static_cast<std::uint8_t>(address), &b, 1, nostop);
  }

  inline std::int32_t i2cReadByte(std::int32_t n, std::int32_t address, bool nostop)
  {
      std::uint8_t b = 0;
      std::int32_t got = i2c_read_blocking(i2cAt(n), static_cast<std::uint8_t>(address), &b, 1, nostop);
      return got == 1 ? static_cast<std::int32_t>(b) : -1;
  }
}
