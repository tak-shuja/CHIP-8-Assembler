start:
  LD I, 0x50 ; Load sprite address at 0x50

  LD V0, 0  ; X position
  LD V1, 10 ; Y position
  LD V7, 5
  ld vc, 0
  ld vd, 0

draw_loop:
  CLS
  DRW V0, V1, 5
  ADD V0, 1
  ADD Vc, 1

  sne vc, 64
  jp  reset_all

  LD V4, 0xFF
  LD DT, V4

wait_delay:
  LD  V5, DT
  SNE V5, 0
  JP  wait_delay
  JP  draw_loop

reset_all:
  ld  vc, 0
  ld  v7, 5
  add I,  v7
  add vd, 1
  sne vd, 0x10
  jp  start
  jp  draw_loop
