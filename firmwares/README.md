# Firmwares

This contains all the firmwares we wrote:

* `mainboard` is self-explanatory.
* `triangle_tester` is a tester for a single triangle, using the STM32F746G Discovery board.
* `nucleo_h7_imu_test` is a tester for the EM7180 IMU, using the STM32H7 Nucleo board.

In addition, `common/` is used for code-sharing. For each shared component,
create a subfolder of `common/` for it, with a Makefile inside like the one in
`common/rtt/`.

Make sure that stuff in `common/` is fully board-independent. To this effect,
components may include a header called, e.g, `rtt_conf.h`, which is to be
defined in the firmwares' folders.
