  .text
  .align 5,0
  .globl _vbr_exp_handler
_vbr_exp_handler:
  nop
  mov #-1,r1
  shll8 r1
  shll16 r1
  mov.l @(0x24,r1),r0
  shlr2 r0
  shlr2 r0
  and #0x7E,r0
  cmp/eq #0x16,r0
  bf expold
  mov.l @(0x20,r1),r0
  shlr2 r0
  extu.b r0,r0
  cmp/eq #0x22,r0
  bt _vbr_syscall_handler
expold:
  mov.l expold_vbr,r0
  mov.l @r0,r0
  add #0x60,r0
  add #0x50,r0
  add #0x50,r0
  jmp @r0
  nop
  .align 2,0
expold_vbr: .long _old_vbr
_vbr_syscall_handler:
  ! save lots of stuff
  stc.l spc,@-r15
  sts.l pr,@-r15
  sts.l mach,@-r15
  sts.l macl,@-r15
  sts.l fpul,@-r15
  fmov.s fr11,@-r15
  fmov.s fr10,@-r15
  fmov.s fr9,@-r15
  fmov.s fr8,@-r15
  fmov.s fr7,@-r15
  fmov.s fr6,@-r15
  fmov.s fr5,@-r15
  fmov.s fr4,@-r15
  fmov.s fr3,@-r15
  fmov.s fr2,@-r15
  fmov.s fr1,@-r15
  fmov.s fr0,@-r15
  stc.l r7_bank,@-r15
  stc.l r6_bank,@-r15
  stc.l r5_bank,@-r15
  stc.l r4_bank,@-r15
  stc.l r3_bank,@-r15
  ! restore original sr (and it better not have been in an int handler)
  stc ssr,r0
  ldc r0,sr
  nop
  ! clear errno
  mova _vbr_syscall_errno,r0
  mov #0,r1
  mov.l r1,@r0
  ! call the real syscall handler
  mov.l syscall_handler_addr,r0
  jsr @r0
  nop
  ! get errno in r1
  mov.l _vbr_syscall_errno,r1
  ! restore lots of stuff
  mov.l @r15+,r3
  mov.l @r15+,r4
  mov.l @r15+,r5
  mov.l @r15+,r6
  mov.l @r15+,r7
  fmov.s @r15+,fr0
  fmov.s @r15+,fr1
  fmov.s @r15+,fr2
  fmov.s @r15+,fr3
  fmov.s @r15+,fr4
  fmov.s @r15+,fr5
  fmov.s @r15+,fr6
  fmov.s @r15+,fr7
  fmov.s @r15+,fr8
  fmov.s @r15+,fr9
  fmov.s @r15+,fr10
  fmov.s @r15+,fr11
  lds.l @r15+,fpul
  lds.l @r15+,macl
  lds.l @r15+,mach
  lds.l @r15+,pr
  ! return (this will trash r2, but who cares)
  mov.l @r15+,r2
  jmp @r2
  nop
  .align 2,0
syscall_handler_addr: .long _syscall_handler
  .globl _vbr_syscall_errno
_vbr_syscall_errno: .long 0
  .align 5,0
  .rep (0x400-0x1C0)
  .byte 0
  .endr
  .align 5,0
_vbr_mmu_handler:
  nop
  rte
  nop
  .align 5,0
  .rep (0x600-0x420)
  .byte 0
  .endr
  .align 5,0
_vbr_int_handler:
  nop
  sts.l pr, @-r15
  sts.l mach, @-r15
  sts.l macl, @-r15
  sts.l fpul, @-r15
  fmov.s fr11,@-r15
  fmov.s fr10,@-r15
  fmov.s fr9,@-r15
  fmov.s fr8,@-r15
  fmov.s fr7,@-r15
  fmov.s fr6,@-r15
  fmov.s fr5,@-r15
  fmov.s fr4,@-r15
  fmov.s fr3,@-r15
  fmov.s fr2,@-r15
  fmov.s fr1,@-r15
  fmov.s fr0,@-r15
  mov.l 0f,r0
  jsr @r0
  nop
  fmov.s @r15+, fr0
  fmov.s @r15+, fr1
  fmov.s @r15+, fr2
  fmov.s @r15+, fr3
  fmov.s @r15+, fr4
  fmov.s @r15+, fr5
  fmov.s @r15+, fr6
  fmov.s @r15+, fr7
  fmov.s @r15+, fr8
  fmov.s @r15+, fr9
  fmov.s @r15+, fr10
  fmov.s @r15+, fr11
  lds.l @r15+, fpul
  lds.l @r15+, macl
  lds.l @r15+, mach
  lds.l @r15+, pr
  rte
  nop
  .align 2,0
0: .long _dcirq_c_handler
