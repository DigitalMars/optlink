;	Static Name Aliases
;
	TITLE   grpdefc.c
	.8087
INCLUDELIB      MLIBCE
INCLUDELIB	OLDNAMES.LIB
GRPDEFC_TEXT	SEGMENT  WORD PUBLIC 'CODE'
GRPDEFC_TEXT	ENDS
_DATA	SEGMENT  WORD PUBLIC 'DATA'
_DATA	ENDS
CONST	SEGMENT  WORD PUBLIC 'CONST'
CONST	ENDS
_BSS	SEGMENT  WORD PUBLIC 'BSS'
_BSS	ENDS
DGROUP	GROUP	CONST, _BSS, _DATA
	ASSUME DS: DGROUP, SS: DGROUP
EXTRN	@get_group:FAR
EXTRN	@next_index:FAR
EXTRN	@install_gindex_lindex:FAR
EXTRN	@next_obj_byte:FAR
EXTRN	@convert_lindex_gindex:FAR
EXTRN	@put_sm_in_group:FAR
_BSS      SEGMENT
COMM NEAR	_obj_record_bytes_left:	BYTE:	 2
COMM NEAR	_group_larray:	BYTE:	 2
COMM NEAR	_segmod_larray:	BYTE:	 2
_BSS      ENDS
GRPDEFC_TEXT      SEGMENT
	ASSUME	CS: GRPDEFC_TEXT
	PUBLIC	@grpdef
@grpdef	PROC FAR
; Line 16
	push	bp
	mov	bp,sp
	sub	sp,2
	push	si
;	group_gindex = -2
;	segmod_gindex = -4
; Line 20
	call	FAR PTR @next_index
	call	FAR PTR @get_group
	mov	si,ax
	lea	bx,WORD PTR _group_larray
	call	FAR PTR @install_gindex_lindex
; Line 21
	cmp	WORD PTR _obj_record_bytes_left,0
	jle	$EX180
$FC184:
; Line 22
	call	FAR PTR @next_obj_byte
	cmp	al,255	;00ffH
	jne	$EX180
; Line 24
	call	FAR PTR @next_index
	lea	bx,WORD PTR _segmod_larray
	call	FAR PTR @convert_lindex_gindex
	mov	dx,ax
	mov	ax,si
	call	FAR PTR @put_sm_in_group
; Line 25
	cmp	WORD PTR _obj_record_bytes_left,0
	jg	$FC184
; Line 27
$EX180:
	pop	si
	mov	sp,bp
	pop	bp
	ret	
	nop	

@grpdef	ENDP
GRPDEFC_TEXT	ENDS
END
