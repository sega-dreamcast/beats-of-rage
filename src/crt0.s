  .text
.globl start
start:
  nop
  stc sr,r3
  mov.l mask_sr,r0
  or r3,r0
  ldc r0,sr
  nop
  mov.l resetcache_addr,r0
  jmp @r0
  nop
resetcache:
  nop
  mov.l ccr_addr,r0
  mov.w ccr_wdata,r1
  mov.l r1,@r0
  nop; nop; nop; nop
  nop; nop; nop; nop
  mov.l init_addr,r0
  jmp @r0
  nop
init:
  nop
  mov.l init_fpscr,r0
  lds r0,fpscr
  mov.l stack_addr,r15
  ldc r3,sr
clearbss:
  mov.l bss_start_addr,r0
  mov.l end_addr,r1
  ! ensure bss clearing is uncached
  mov.l p2_addr,r2
  or r2,r0
  or r2,r1
  ! if r0 >= r1 goto main
  cmp/hs r1,r0
  bt/s gotomain
  mov #0,r2
clearbss_loop:
  mov.l r2,@-r1
  cmp/hs r1,r0
  bf clearbss_loop
gotomain:
  mov.l main_addr,r0
  jsr @r0
  nop
  mov r0,r4
  mov.l exit_addr,r0
  jsr @r0
  nop
endlessloop:
  nop
  bra endlessloop
  nop
  .align 1,0
ccr_wdata:       .word 0x090D
  .align 2,0
ccr_addr:        .long 0xFF00001C
mask_sr:         .long 0x100000F0
resetcache_addr: .long 0xAC010000+(resetcache-start)
init_addr:       .long 0x8C010000+(init-start)
init_fpscr:      .long 0x00040000
bss_start_addr:  .long __bss_start
end_addr:        .long _end
stack_addr:      .long _stack
main_addr:       .long _main
exit_addr:       .long _exit
p2_addr:         .long 0xA0000000
