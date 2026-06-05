.section .text._start  @ Sección específica para el inicio
.global _start
_start:
    bl main
hang:
    b hang
