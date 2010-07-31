;; Japanese translation, 1/94.
;; Please check entries with "???".
;; no translate "debug" and "os2" messages.

TITLE	ERRORS - Copyright (C) SLR Systems 1988


SHORT_JMP	DB	LENGTH SHORT_JMP-1,'Short JMP'
;SHORT_JMP	DB	LENGTH SHORT_JMP-1,'Short JMP'
NORMAL_JMP	DB	LENGTH NORMAL_JMP-1,'Near JMPまたはCALL'
;NORMAL_JMP	DB	LENGTH NORMAL_JMP-1,'Near JMP or CALL'
UNDEF_JMP	DB	LENGTH UNDEF_JMP-1,'未定の型'
;UNDEF_JMP	DB	LENGTH UNDEF_JMP-1,'Unknown Type'
NORMAL_32_JMP	DB	LENGTH NORMAL_32_JMP-1,'32ビット条件JMP'
;NORMAL_32_JMP	DB	LENGTH NORMAL_32_JMP-1,'32-bit Conditional JMP'
LOBYTE_ADR1	DB	LENGTH LOBYTE_ADR1-1,'LoByte (単一バイト)'
;LOBYTE_ADR1	DB	LENGTH LOBYTE_ADR1-1,'LoByte (single byte)'
OFFSET_ADR1	DB	LENGTH OFFSET_ADR1-1,'16ビットオフセット'
;OFFSET_ADR1	DB	LENGTH OFFSET_ADR1-1,'16-bit Offset'
WOFFSET_ADR1	DB	LENGTH WOFFSET_ADR1-1,'16ビット破壊オフセット'
;WOFFSET_ADR1	DB	LENGTH WOFFSET_ADR1-1,'16-bit Weird Offset'
BASE_ADR1	DB	LENGTH BASE_ADR1-1,'セグメントベース'
;BASE_ADR1	DB	LENGTH BASE_ADR1-1,'Segment Base'
PTR_ADR1	DB	LENGTH PTR_ADR1-1,'DWORD セグメント:オフセット'
;PTR_ADR1	DB	LENGTH PTR_ADR1-1,'DWORD seg:offs'
HIBYTE_ADR1	DB	LENGTH HIBYTE_ADR1-1,'16ビットオフセットの上位バイト'
;HIBYTE_ADR1	DB	LENGTH HIBYTE_ADR1-1,'Hi Byte of 16-bit Offset'
DWORD_ADR1	DB	LENGTH DWORD_ADR1-1,'32ビットオフセット'
;DWORD_ADR1	DB	LENGTH DWORD_ADR1-1,'32-bit Offset'
WDWORD_ADR1	DB	LENGTH WDWORD_ADR1-1,'32ビット破壊オフセット'
;WDWORD_ADR1	DB	LENGTH WDWORD_ADR1-1,'32-bit Weird Offset'
FWORD_PTR_ADR1	DB	LENGTH FWORD_PTR_ADR1-1,'FWORD セグメント:32ビットオフセット'
;FWORD_PTR_ADR1	DB	LENGTH FWORD_PTR_ADR1-1,'FWORD seg:32-bit offs'

TARG_ADR	DB	'TARGET'
;TARG_ADR	DB	'TARGET'
LOC_ADR 	DB	'現在位置'
;LOC_ADR 	DB	'Current Location'
EXTERNAL_ADR	DB	'外部シンボル '
;EXTERNAL_ADR	DB	'External Symbol '
GROUP_ADR	DB	'グループ '
;GROUP_ADR	DB	'Group '
SEGMENT_ADR	DB	'セグメント '
;SEGMENT_ADR	DB	'Segment '

;; ??? assuming that the following strings are CONCATENATED when output.
;; ??? please check.

FRAME_ADR	DB	' FRAME  = '
;FRAME_ADR	DB	' FRAME  = Frame of '
TARGET_ADR	DB	' TARGET = '
;TARGET_ADR	DB	'のフレーム、TARGET = '
ATRELATIVE_ADR	DB	' '
;ATRELATIVE_ADR	DB	'at Relative '
FROMSEG_ADR	DB	' 相対',0DH,0AH,'セグメント'
;FROMSEG_ADR	DB	' from',0DH,0AH,' Segment '
LINE_ADR	DB	'、およその行番号'
;LINE_ADR	DB	' Near Line #'
INFILE_ADR	DB	'、ファイル'
;INFILE_ADR	DB	' in File '
FIX_TYPE_ADR	DB	'、FIXUPP タイプ = '
;FIX_TYPE_ADR	DB	' FIXUPP Type = '


.XALL
ERR_CNT 	=	0
DEFINE		MACRO	XX,YY
T&XX		EQU	YY
		ENDM

MK		MACRO	XX,YY
		LOCAL	L1,L2
		PUBLIC	XX
XX		EQU	ERR_CNT+0
L1		DB	L2,YY
L2		EQU	$-L1-1
		DEFINE	%ERR_CNT,L1
ERR_CNT 	=	ERR_CNT+1
		ENDM

BUILD		MACRO	XX
		DW	T&XX
		ENDM

MK	OOPS_ERR,'まいった。'
;MK	OOPS_ERR,'Oops'

if	fg_demo
MK	DEMO_EXPIRED_ERR,'このデモ版が期限切れになりました。'
;MK	DEMO_EXPIRED_ERR,'This DEMO has expired.'
endif

;Print symbol name for these error msgs
MK	PREV_DEF_ERR,'前の定義と違う'            ;error
;MK	PREV_DEF_ERR,'Previous Definition Different'            ;error

;Print file name for these errors
MK	FILE_NOT_FOUND_ERR,'ファイルが見つからない'                     ;fatal for objs and indirect, warn for libs
;MK	FILE_NOT_FOUND_ERR,'File Not Found'                     ;fatal for objs and indirect, warn for libs
;MK	SYM_LONG_ERR,'Symbol Name Too Long'                     ;fatal
MK	CANT_CREATE_ERR,'ファイルを作成できない'                    ;fatal
;MK	CANT_CREATE_ERR,'Cannot Create File'                    ;fatal
MK	REC_TOO_LONG_ERR,'OBJレコードが長すぎる'                  ;fatal
;MK	REC_TOO_LONG_ERR,'OBJ Record Too Long'                  ;fatal
MK	BAD_CHKSUM_ERR,'チェックサムが不正'                           ;warning
;MK	BAD_CHKSUM_ERR,'Bad Checksum'                           ;warning
MK	LIBRARY_ERR,'ライブラリエラー'				;error
;MK	LIBRARY_ERR,'Library Error'				;error
;-----------------------------------
if	fgh_os2
MK	OOM_ERR,'Swap File Full'				;fatal
else
MK	OOM_ERR,'メモリ不足'                                 ;fatal
;MK	OOM_ERR,'Out of Memory'                                 ;fatal
endif
MK	FILENAME_ERR,'不正ファイル名'                         ;fatal
;MK	FILENAME_ERR,'Illegal Filename'                         ;fatal
MK	CMDLIN_ERR,"不正オプション"                             ;fatal
;MK	CMDLIN_ERR,"Unknown option"                             ;fatal
MK	DISK_FULL_ERR,'ディスクの残り容量がない'                       ;fatal
;MK	DISK_FULL_ERR,'Disk Full Writing'                       ;fatal
MK	MISSING_COLON_ERR,'コロンが必要'                      ;fatal
;MK	MISSING_COLON_ERR,'Colon Expected'                      ;fatal
MK	ILL_NUMBER_ERR,'数値オーバーフロー'                        ;fatal
;MK	ILL_NUMBER_ERR,'Number Overflow'                        ;fatal
MK	OBJ_PHASE_ERR,'レコードのシンタックスが不正'			;FATAL
;MK	OBJ_PHASE_ERR,'Illegal Record Syntax'			;FATAL
MK	BAD_RECORD_ERR,'レコードが認識できない'			;ERROR
;MK	BAD_RECORD_ERR,'Unrecognized Record'			;ERROR
; ??? what does extension mean ???
MK	BAD_OMF_EXTENT_ERR,'OMF拡張子が不正'			;fatal
;MK	BAD_OMF_EXTENT_ERR,'Bad OMF Extension'			;fatal
; ??? what does index range mean?
MK	INDEX_RANGE_ERR,'インデックスの範囲'				;FATAL
;MK	INDEX_RANGE_ERR,'Index Range'				;FATAL

MK	GRP_TOO_BIG_ERR,'グループのサイズが64Kを越えている'		;ERROR
;MK	GRP_TOO_BIG_ERR,'Group Size Exceeds 64k'		;ERROR
MK	GRP_TYP_ERR,'GROUP に再配置可能とアブソリュートは同時に指定できない';ERROR
;MK	GRP_TYP_ERR,'GROUP Cannot Be Both Relocatable and Absolute';ERROR

MK	SEG_TOO_BIG_ERR,'セグメントのサイズが64Kを越えている'		;ERROR
;MK	SEG_TOO_BIG_ERR,'Segment Size Exceeds 64k'		;ERROR
MK	SEG_4G_ERR,'セグメント >= 4G'				;ERROR
;MK	SEG_4G_ERR,'Segment >= 4G'				;ERROR
MK	BAD_SEG_SIZE_ERR,'セグメントサイズがおかしい'		;ERROR
;MK	BAD_SEG_SIZE_ERR,'Badly Formed Segment Size'		;ERROR
MK	NC_TOO_BIG_ERR,'Near Communalサイズが64kを越えている'	;FATAL
;MK	NC_TOO_BIG_ERR,'Near Communal Size Exceeds 64k'		;FATAL
MK	DUP_GROUP_ERR,'セグメントが既に別のグループにある'	;WARNING
;MK	DUP_GROUP_ERR,'Segment Already in Different Group'	;WARNING

MK	MISSING_STACK_ERR,'スタックがない'				;WARNING
;MK	MISSING_STACK_ERR,'No Stack'				;WARNING
MK	MULTIPLE_STACKS_ERR,'モジュールにスタックが既に定義されている'	;WARNING
;MK	MULTIPLE_STACKS_ERR,'Stack Already Declared in Module'	;WARNING

MK	MUST_HAVE_SEGMENT_ERR,'PUBDEFにはセグメントインデックスがなければならない'	;ERROR
;MK	MUST_HAVE_SEGMENT_ERR,'PUBDEF Must Have Segment Index'	;ERROR
MK	TOO_COMPLEX_ERR,'モジュール内のGroups, Segments, Externalsが多すぎる'
;MK	TOO_COMPLEX_ERR,'Too Many Groups, Segments or Externals in Module'

MK	LINE_TOO_LONG_ERR,'1行が500バイトを越えている'
;MK	LINE_TOO_LONG_ERR,'Line Exceeds 500 Bytes Reading '
MK	BAD_THREAD_ERR,'FIXUPPスレッドが不正'			;FATAL
;MK	BAD_THREAD_ERR,'Bad FIXUPP Thread'			;FATAL
MK	BAD_LOC_ERR,'開始アドレスのLOCフレームが不正'		;FATAL
;MK	BAD_LOC_ERR,'Bad LOC Frame in start address'		;FATAL

MK	DOS_ERR,'DOS 重大エラー'				;FATAL
;MK	DOS_ERR,'DOS Critical Error'				;FATAL

MK	UNEXP_EOF_ERR,'ファイルの終りに達してしまった'			;FATAL
;MK	UNEXP_EOF_ERR,'Unexpected End of File'			;FATAL
MK	TOO_MANY_SYMBOLS_ERR,'LIBファイル内のモジュールが多すぎる'
;MK	TOO_MANY_SYMBOLS_ERR,'Too Many Modules in LIB File'
MK	MUL32_ERR,'32ビット乗算オーバーフロー'			;ABORT
;MK	MUL32_ERR,'Overflow 32-bit Multiply'			;ABORT
MK	NOT_IN_GROUP_ERR,'Group外のセグメント'
;MK	NOT_IN_GROUP_ERR,'Segment Not in That Group'

MK	DATA_OUTSIDE_SEGMOD_ERR,'セグメント範囲外のデータがある'	;ERROR
;MK	DATA_OUTSIDE_SEGMOD_ERR,'Data Outside Segment Bounds'	;ERROR

MK	UNRECOGNIZED_FIXUPP_FRAME_ERR,'不明なFIXUPPフレームタイプ';FATAL
;MK	UNRECOGNIZED_FIXUPP_FRAME_ERR,'Unknown FIXUPP Frame Type';FATAL

MK	FIXUPP_OFFSET_ERR,'FIXUPPはデータレコードの先を指している'	;error
;MK	FIXUPP_OFFSET_ERR,'FIXUPP Points Past Data Record'	;error
MK	CANT_REACH_ERR,'FRAMEからTARGETにはたどりつけない'		;error
;MK	CANT_REACH_ERR,'Cannot Reach TARGET from FRAME'		;error
MK	LOC_FRAME_ERR,'LOCATIONがFRAME内ではない'		;error
;MK	LOC_FRAME_ERR,'LOCATION Not Within FRAME'		;error
MK	SHORT_ERR,'範囲外のShort JMP'			;error
;MK	SHORT_ERR,'Short JMP Out of Range'			;error
MK	LOBYTE_ERR,'範囲外のBYTE'				;error
;MK	LOBYTE_ERR,'BYTE Out of Range'				;error
MK	BASE_RELOC_ERR,'絶対モードでは再配置可能なベースを利用できない';error
;MK	BASE_RELOC_ERR,'Relocatable Bases Not Allowed in Absolute Mode';error
MK	TOO_MANY_BASES_ERR,'ベースFIXUPPが65535を越えている';fatal
;MK	TOO_MANY_BASES_ERR,'Limit of 65535 Base Fixupps Exceeded';fatal
MK	UNREC_FIXUPP_ERR,'不明なFIXUPPタイプ'		;error
;MK	UNREC_FIXUPP_ERR,'Unrecognized FIXUPP Type'		;error
MK	ASEG_RELOC_ERR,'ASEGからRELOCへ、またはRELOCからASEGへはたどりつけない'
;MK	ASEG_RELOC_ERR,'Cannot Reach ASEG from RELOC or RELOC from ASEG'
MK	UNDEFINED_ERR,'未定義シンボル'			;error
;MK	UNDEFINED_ERR,'Symbol Undefined'			;error
MK	MULTI_CSEG_ERR,'一つのモジュール内に複数のCODEセグメントが存在する'
;MK	MULTI_CSEG_ERR,'Multiple CODE Segments in Single Module'
MK	LIB_ERR,'不正ライブラリファイル'			;fatal
;MK	LIB_ERR,'Not a Valid Library File'			;fatal
MK	DICT_TOO_BIG_ERR,'LIB辞書が多きすぎる'
;MK	DICT_TOO_BIG_ERR,'LIB Dictionary Too Big'
MK	MIC_NUM_ERR,'Communalシンタックスを認識できない'		;fatal
;MK	MIC_NUM_ERR,'Unrecognized Communal Syntax'		;fatal
MK	CV_TOO_MANY_ERR,'旧CodeView形式にはデバッグ情報が多過ぎる';fatal
;MK	CV_TOO_MANY_ERR,'Too Much DEBUG Data for Old CodeView format';fatal
MK	COM_PC_ERR,'開始アドレスは100Hでなければならない'			;error
;MK	COM_PC_ERR,'Start Address Must Be 100H'			;error
MK	COM_BELOW_100_ERR,'100H以下は初期化できない'	;warning
;MK	COM_BELOW_100_ERR,'Below 100H Cannot Be Initialized'	;warning
MK	COM_ABOVE_100_ERR,'100Hより上には何もない'
;MK	COM_ABOVE_100_ERR,'Nothing Above 100H'
MK	FORREF_ERR,'B2レコードを認識できない'			;fatal
;MK	FORREF_ERR,'Unrecognized B2 Record'			;fatal
MK	CTRLC_ERR,'ユーザによる中止'					;fatal
;MK	CTRLC_ERR,'User ABORT'					;fatal
iff	fgh_os2
MK	CANT_REOPEN_ERR,'出力ファイルを再オープンできない'		;fatal
;MK	CANT_REOPEN_ERR,'Cannot Reopen output file'		;fatal
endif
if	fg_virt
MK	EMS_ERR,'         EMSエラー'				;fatal
;MK	EMS_ERR,'         EMS Error'				;fatal
MK	EMS_CANT_ALLOC_ERR,'EMSブロックを確保できない'		;warning
;MK	EMS_CANT_ALLOC_ERR,'Cannot Allocate EMS Blocks'		;warning
MK	XMS_ERR,'XMS エラー'					;fatal
;MK	XMS_ERR,'XMS Error'					;fatal
endif
MK	CURN_MOD_ERR,'SEGDEF Curnmod が定義されていない'
;MK	CURN_MOD_ERR,'SEGDEF Curnmod Undefined'
MK	DUP_MODEND_ERR,'モジュールのstartがすでに指定されている';warning
;MK	DUP_MODEND_ERR,'Start Previously Specified in Module'	;warning
MK	CV_TOO_MANY_LINNUMS_ERR,'モジュール内の行番号デバック情報が多すぎる';fatal
;MK	CV_TOO_MANY_LINNUMS_ERR,'Too Many Linenumbers in a Module for Debug Info';fatal
MK	SYNTAX_ERR,'.DEFシンタックスエラー'			;fatal
;MK	SYNTAX_ERR,'.DEF Syntax Error'				;fatal
if	fg_rom
MK	ALREADY_ORGED_ERR,'既に整列されている'
;MK	ALREADY_ORGED_ERR,'Already ordered'
MK	ASEG_ERR,'不正SEGMENT。レコード: '
;MK	ASEG_ERR,'Illegal SEGMENT AT record'
MK	SEG_OVERLAP_ERR,'セグメントが別のセグメントと重なっている'
;MK	SEG_OVERLAP_ERR,'Segment Overlaps Another'
; ??? what is a hole ???
MK	CANNOT_PLACE_ERR,'セグメント用の穴が見つからない'
;MK	CANNOT_PLACE_ERR,'Cannot find hole for segment'
MK	SEG_WRAP_ERR,'セグメントが1GB境界を跨っている'
;MK	SEG_WRAP_ERR,'Segment wraps 1 Mb Boundary'
MK	LEFT_ERR,"'('がなければならない"
;MK	LEFT_ERR,"'(' Expected"
MK	RIGHT_ERR,"コンマまたは')'がなければならない"
;MK	RIGHT_ERR,"Comma or ')' Expected"
MK	RANGE_WRAP_ERR,'不正な領域の重なり'
;MK	RANGE_WRAP_ERR,'Illegal Range-wrap'
MK	MUST_SEGMENT_ERR,'PhaseはSEGMENTごとでなければならない'
;MK	MUST_SEGMENT_ERR,'Phase must be by SEGMENT'
MK	ALREADY_PHASED_ERR,'既にPHASE化されている'
;MK	ALREADY_PHASED_ERR,'Already PHASEd'
MK	TOO_PHASE_ERR,'PHASE化セグメントが多過ぎる'
;MK	TOO_PHASE_ERR,'Too many PHASEd segments'
MK	OUTTYPE_UNDEF_ERR,'未定義出力タイプ'
;MK	OUTTYPE_UNDEF_ERR,'Undefined Output type'
endif
MK	START_ERR,'不正開始アドレス'			;error
;MK	START_ERR,'Illegal Start Address'			;error
if	debug
MK	LIB1_ERR,'BINSER_MODULEでモジュールが見つかりません'
;MK	LIB1_ERR,'BINSER_MODULE cannot find module'
MK	UNLOCK_ERR,'非ロックのブロックをロック解除しようとした'
;MK	UNLOCK_ERR,'Attempt to unlock unlocked block'
endif
;MK	LIB2_ERR,'Requesting a Block with no Modules...'


if	fgh_os2

MK	THREAD_FAIL_ERR,'Cannot Create Thread'			;fatal
;MK	BAD_SEM_ERR,'Semaphore Error'				;fatal
MK	TIMEOUT_ERR,'Semaphore Timeout'				;fatal
;MK	PRIORITY_ERR,'Cannot Set Priority'			;fatal
MK	QFH_FAILED_ERR,'QFH Error'				;fatal
MK	CLOSE_ERR,'Close Error'					;fatal
MK	DOS_READ_ERR,'Critical Read'				;fatal
MK	DOS_POS_ERR,'Critical Position'				;fatal
MK	DOS_WRITE_ERR,'Critical Write'				;fatal
MK	DOS_NEWSIZE_ERR,'DOSNEWSIZE Error'			;fatal
MK	DOS_SETFILEINFO_ERR,'DOSSETFILEINFO Error'		;fatal
;MK	UNEXP_XEOF_ERR,'     Unexpected Extended End of File'

endif

MK	SEG_ALREADY_RELEASED_ERR,'セレクタが既に解放されている'	;fatal
;MK	SEG_ALREADY_RELEASED_ERR,'Selector Already Released'	;fatal
MK	RELOC_CONFLICT_ERR,'データと再配置がオーバラップしている'	;error
;MK	RELOC_CONFLICT_ERR,'Data Overlaps Relocations'		;error
MK	CANNOT_PACK_ERR,'再配置が多すぎてEXEPACKできない'		;fatal
;MK	CANNOT_PACK_ERR,'Too Many Relocs to EXEPACK'		;fatal
MK	NOTHING_ERR,'どのセグメントもリンクしなかった'			;fatal
;MK	NOTHING_ERR,'No Segments Linked!'			;fatal
MK	INCERR_ERR,'インクリメンタルコンパイルのエラー'			;fatal
;MK	INCERR_ERR,'Incremental Compile Error'			;fatal
MK	EXESTR_ERR,'EXESTRデータが多過ぎる'			;fatal
;MK	EXESTR_ERR,'Too much EXESTR data'			;fatal
MK	ALIAS_ERR,'不正なALIAS'				;error
;MK	ALIAS_ERR,'Illegal ALIAS'				;error
MK	CIRCULAR_ALIAS_ERR,'ALIASが循環している'			;fatal
;MK	CIRCULAR_ALIAS_ERR,'Circular ALIAS'			;fatal

MK	SEG_COMMON_ERR,'COMMONの組合せタイプが他をオーバライドする'	;warning
;MK	SEG_COMMON_ERR,'COMMON Combine type overrides others'	;warning
MK	SEG_STACK_ERR,'STACKの組合せタイプが他をオーバライドしている';warning
;MK	SEG_STACK_ERR,'STACK Combine type overrides others'	;warning

MK	LIN_NONCODE_ERR,'非CODEセグメントにLINNUMがあった'	;warning
;MK	LIN_NONCODE_ERR,'LINNUMs in Non-CODE segment'		;warning
MK	LINSYM_BEFORE_COMDAT_ERR,'COMDATの前にLINSYMがある'	;ERROR
;MK	LINSYM_BEFORE_COMDAT_ERR,'LINSYM before COMDAT'		;ERROR
MK	LINSYM_CONT_ERR,'LINSYM が継続している'			;ERROR
;MK	LINSYM_CONT_ERR,'LINSYM Continuation'			;ERROR

if	fg_segm

MK	CODEPACK_ERR,'セグメントが多すぎるのでPACKCODEを試行'	;warning
;MK	CODEPACK_ERR,'Too many segments, trying PACKCODE'	;warning
MK	DATAPACK_ERR,'セグメントが多すきるのでPACKDATAを試行'	;warning
;MK	DATAPACK_ERR,'Too many segments, trying PACKDATA'	;warning
MK	TOO_MANY_SEGS_ERR,'セグメント化EXE形式にするにはセグメントが多すぎる'	;fatal
;MK	TOO_MANY_SEGS_ERR,'Too many segments for segmented .EXE format'	;fatal
MK	TOO_ENTRIES_ERR,'ENTRYが多過ぎる'			;fatal
;MK	TOO_ENTRIES_ERR,'Too many ENTRIES'			;fatal
MK	EXP_CONST_ERR,'EXPORTできない'				;error
;MK	EXP_CONST_ERR,'Cannot EXPORT'				;error
MK	DUP_ENT_ORD_ERR,'ORDINAL番号が重複している'		;error
;MK	DUP_ENT_ORD_ERR,'Duplicate ORDINAL Number'		;error
MK	START_CANT_REACH_ERR,'開始アドレスのフレームが不正'	;error
;MK	START_CANT_REACH_ERR,'Illegal frame on start address'	;error
MK	START_IMPORT_ERR,'IMPORTを開始アドレスには使えない'	;error
;MK	START_IMPORT_ERR,'IMPORT illegal as start address'	;error
MK	BAD_STUB_ERR,'不正STUBファイル'				;fatal
;MK	BAD_STUB_ERR,'Bad STUB file'				;fatal
MK	EXPORT_CONFLICT_ERR,'_exportとEXPORTSの矛盾'	;error
;MK	EXPORT_CONFLICT_ERR,'_export conflicts with EXPORTS'	;error
MK	ALIGN_SMALL_ERR,'/ALIGNMENT値を大きくする必要がある'			;fatal
;MK	ALIGN_SMALL_ERR,'Need larger /ALIGNMENT value'		;fatal
MK	HEAP_STACK_DGROUP_ERR,'DGROUP+スタック+ヒープが64k-16を越えている'	;error
;MK	HEAP_STACK_DGROUP_ERR,'DGROUP + Stack + Heap exceeds 64k-16';error
MK	GRP_OVERLAP_ERR,'グループがオーバラップしている'	;error
;MK	GRP_OVERLAP_ERR,'Groups Overlap'			;error
MK	ONE_NAME_ERR,'NAMEまたはLIBRARYは1つに限る'		;error
;MK	ONE_NAME_ERR,'Only one NAME or LIBRARY allowed'		;error
MK	SEGM_COM_SYS_ERR,'セグメント化.COMまたは.SYSファイルが生成できない'	;warning
;MK	SEGM_COM_SYS_ERR,'Cannot generate Segmented .COM or .SYS file'	;warning
MK	DLL_NO_LIB_ERR,'.DLL用の.DEFファイルのLIBRARY指定が欠けている'	;warning
;MK	DLL_NO_LIB_ERR,'Missing LIBRARY in .DEF file for .DLL'	;warning
MK	ONE_DESC_ERR,'複数の記述'			;error
;MK	ONE_DESC_ERR,'Multiple Descriptions'			;error
MK	DEF_CODE_ERR,'CODE指令'				;error
;MK	DEF_CODE_ERR,'CODE Directive'				;error
MK	DEF_DATA_ERR,'DATA指令'				;error
;MK	DEF_DATA_ERR,'DATA Directive'				;error
MK	DEF_SEGMENTS_ERR,'SEGMENTS指令'			;error
;MK	DEF_SEGMENTS_ERR,'SEGMENTS Directive'			;error
MK	DUP_HEAP_ERR,'HEAPSIZE指令'			;error
;MK	DUP_HEAP_ERR,'HEAPSIZE Directive'			;error
MK	DEF_EXETYPE_ERR,'EXETYPE指令'			;error
;MK	DEF_EXETYPE_ERR,'EXETYPE Directive'			;error
MK	PROT_REAL_ERR,'PROT vs REAL conflict'			;error
;MK	PROT_REAL_ERR,'PROTとREALが矛盾している'			;error
MK	DEF_IMPORTS_ERR,'IMPORTS指令'			;error
;MK	DEF_IMPORTS_ERR,'IMPORTS Directive'			;error
MK	DEF_EXPORTS_ERR,'EXPORTS指令'			;error
;MK	DEF_EXPORTS_ERR,'EXPORTS Directive'			;error
MK	DUP_OLD_ERR,'OLDが重複している'				;error
;MK	DUP_OLD_ERR,'Duplicate OLD'				;error
MK	DUP_STUB_ERR,'STUBが重複している'			;error
;MK	DUP_STUB_ERR,'Duplicate STUB'				;error
MK	DEF_STRING_ERR,'区切り文字列でなければならない'		;error
;MK	DEF_STRING_ERR,'Delimited String Expected'		;error
MK	APPLOAD_ERR,'APPLOADはSEGMENTSの前でなければならない'	;fatal
;MK	APPLOAD_ERR,'APPLOAD must appear before any SEGMENTS'	;fatal
MK	DUP_RESOURCE_ERR,'RESOURCEが重複しているので無視'		;error
;MK	DUP_RESOURCE_ERR,'Duplicate RESOURCE, ignored'		;error
MK	MULTIPLE_RC_ERR,'RCコマンドが重複している'			;error
;MK	MULTIPLE_RC_ERR,'Duplicate RC commands'			;error
MK	RC_64K_ERR,'Preloadセグメント+再配置 > 64kなので、-kオプションをつける必要がある'	;error
;MK	RC_64K_ERR,'Preload Segment + Relocs > 64k, use -k'	;error
MK	EXEHDR_ERR,'.EXEヘッダ >64k'				;error
;MK	EXEHDR_ERR,'.EXE Header >64k'				;error
MK	RESOURCE_CORRUPT_ERR,'.RESファイルが破壊されている'		;fatal
;MK	RESOURCE_CORRUPT_ERR,'.RES file Corrupt'		;fatal
MK	EXP_TEXT_ERR,'EXPORT名のテキスト>64k'			;fatal
;MK	EXP_TEXT_ERR,'EXPORTed names text >64k'			;fatal
MK	SUBSYSTEM_ERR,'SUBSYSTEM指令'			;error
;MK	SUBSYSTEM_ERR,'SUBSYSTEM Directive'			;error

endif

iff	fg_segm

MK	NO_IMPORTS_ERR,'不正なImport宣言'	;fatal
;MK	NO_IMPORTS_ERR,'Import Declarations Illegal'	;fatal

endif

if	fg_prot

MK	OS2_FLAG_CONFLICT_ERR,'グループ化セグメントのフラグが矛盾している'	;warning
;MK	OS2_FLAG_CONFLICT_ERR,'Grouped segments have conflicting flags'	;warning
MK	GRP_COD_DAT_ERR,'グループは同時にCODEとDATAのセグメントを含めない'	;error
;MK	GRP_COD_DAT_ERR,'Group cannot contain CODE and DATA segments'	;error

endif

MK	TOKEN_TOO_LONG_ERR,'トークンが長過ぎる'		;fatal
;MK	TOKEN_TOO_LONG_ERR,'Token Too Long'		;fatal
MK	FILE_EXP_ERR,'ファイル名でなければならない'		;fatal
;MK	FILE_EXP_ERR,'Filename Expected'		;fatal
;; ??? what does this meam?
MK	WEAK_DEF_ERR,'Weak EXTRNが違う'		;error
;MK	WEAK_DEF_ERR,'Weak EXTRN Different'		;error

MK	INDIRECT_DEEP_ERR,'間接ファイルのネストが深すぎる';fatal
;MK	INDIRECT_DEEP_ERR,'Indirect file nested too deep';fatal
MK	GRP_ERR,'未サポートGRPDEFのタイプ'		;error
;MK	GRP_ERR,'Unsupported GRPDEF Type'		;error
MK	ALIAS_IGNORED_ERR,'ALIASは既に定義されている'	;warn
;MK	ALIAS_IGNORED_ERR,'ALIAS Previously defined'	;warn
MK	CANNOT_LINK_ERR,'リンクできない'			;fatal
;MK	CANNOT_LINK_ERR,'Cannot Link'			;fatal

if	any_overlays

MK	MISSING_ENDAREA_ERR,'ENDAREAが欠けている'		;fatal
;MK	MISSING_ENDAREA_ERR,'Missing ENDAREA'		;fatal
MK	ENDAREA_ERR,'ENDAREAに対応するBEGINAREAがない'		;fatal
;MK	ENDAREA_ERR,'ENDAREA without BEGINAREA'		;fatal
MK	CACHE_VALUE_ERR,'キャッシュ値'			;fatal
;MK	CACHE_VALUE_ERR,'Cache Value'			;fatal
MK	DUP_CACHE_ERR,'キャッシュ要求が重複している'		;fatal
;MK	DUP_CACHE_ERR,'Duplicate Cache Request'		;fatal
MK	SECTION_REQ_ERR,'Overlayセクション内でなければならない'	;fatal
;MK	SECTION_REQ_ERR,'Must be in Overlay Section'	;fatal
MK	PAREN_NESTING_ERR,'括弧が対応していない'		;fatal
;MK	PAREN_NESTING_ERR,'Mismatched Parens'		;fatal

endif

MK	ID_EXP_ERR,'シンボルまたは名称でなければならない'		;fatal
;MK	ID_EXP_ERR,'Symbol or Name Expected'		;fatal

if	fg_plink

MK	RIGHT_ERR,"','または')'がなければならない"		;fatal
;MK	RIGHT_ERR,"Comma or ')' Expected"		;fatal
MK	ALWAYS_NEVER_CONFLICT_ERR,'Always-Neverの矛盾';fatal
;MK	ALWAYS_NEVER_CONFLICT_ERR,'Always-Never Conflict';fatal
MK	AREA_DEEP_ERR,'BEGINAREAのネストが深過ぎる'	;fatal
;MK	AREA_DEEP_ERR,'BEGINAREAs nested too deep'	;fatal
MK	TRACK_NEVER_ERR,'Track-Neverの矛盾'		;fatal
;MK	TRACK_NEVER_ERR,'Track-Never Conflict'		;fatal
MK	NEAR_FAR_ERR,'Near-Farでなければならない'		;fatal
;MK	NEAR_FAR_ERR,'Near-Far Expected'		;fatal
MK	PUB_NOPUB_ERR,'Public-Nopublicの矛盾'	;fatal
;MK	PUB_NOPUB_ERR,'Public-Nopublics Conflict'	;fatal
MK	FI_MO_SY_ERR,'ファイル-モジュール-シンボルでなければならない'	;fatal
;MK	FI_MO_SY_ERR,'File-Module-Symbol Expected'	;fatal

endif
if	any_overlays

MK	DUP_SECTION_ERR,'その項目には既にセクションが宣言されている'	;fatal
;MK	DUP_SECTION_ERR,'That Item Already has Section Declared'	;fatal
MK	TOO_MANY_VECTORS_ERR,'ベクトル化シンボルが多過ぎる';fatal
;MK	TOO_MANY_VECTORS_ERR,'Too Many Vectored Symbols';fatal
MK	OVL_COM_SYS_ERR,'COMまたはSYSのオーバレイができない。extentをEXEに変更する';warning
;MK	OVL_COM_SYS_ERR,'Cannot Overlay COM or SYS files, extent changed to EXE';warning

endif

if	debug
MK	EMS_XMS_LIST_ERR,'MRU or Freelist corrupt'
endif

MK	SECTION_ERR,'オーバレイ番号 >32k'			;error
;MK	SECTION_ERR,'Overlay Number >32k'			;error
MK	COMDAT_SYNTAX_ERR,'COMDATシンタックス'			;fatal
;MK	COMDAT_SYNTAX_ERR,'COMDAT Syntax'			;fatal
MK	COMDAT_CONT_ERR,'COMDAT継続行がおかしい'		;fatal
;MK	COMDAT_CONT_ERR,'COMDAT Continuation Mismatch'		;fatal
MK	BAD_CD_ALLOC_ERR,'認識できないCOMDAT割り当てタイプ'	;fatal
;MK	BAD_CD_ALLOC_ERR,'Unknown COMDAT Allocation type'	;fatal
; ??? what does this mean?
MK	DEF_WEAK_INT_ERR,'WeakLazyAlias Internal'		;fatal
;MK	DEF_WEAK_INT_ERR,'WeakLazyAlias Internal'		;fatal
MK	COMMUNAL_ERR,'COMDEFのサイズが不正'			;warn
;MK	COMMUNAL_ERR,'Bad COMDEF Sizes'			;warn
MK	CEXTDEF_ERR,'COMDATがないのにCEXTDEFがあった'		;fatal
;MK	CEXTDEF_ERR,'CEXTDEF with no COMDAT'		;fatal
MK	CONST_FRAME_ERR,'定数はFRAME=0でなければならない'	;error
;MK	CONST_FRAME_ERR,'Constants must have FRAME=0'	;error

MK	STD_MAXINDEX_ERR,'64kの制限を越えた: '		;fatal
;MK	STD_MAXINDEX_ERR,'64k Limit Exceeded:'		;fatal
MK	NO_START_ERR,'開始アドレスがない'			;warn
;MK	NO_START_ERR,'No Start Address'			;warn

MK	UNREC_IN_CFG_ERR,'.CFGに認識できないデータがある'	;ERROR
;MK	UNREC_IN_CFG_ERR,'Unrecognized data in .CFG'	;ERROR
MK	LIB_DUP_ERR,'このLIBで定義されたシンボルは他でも定義されている'	;warn
;MK	LIB_DUP_ERR,'Symbol in this LIB Defined Elsewhere'	;warn
MK	HEAP_NO_DGROUP_ERR,'DGROUPがないのにヒープがあった'	;warning
;MK	HEAP_NO_DGROUP_ERR,'Heap without DGROUP'	;warning

if	fg_td

MK	TD_E0_EXTDEF_ERR,'EXTDEFのないE0'		;ERROR
;MK	TD_E0_EXTDEF_ERR,'E0 without EXTDEF'		;ERROR
MK	TD_E1_PUBDEF_ERR,'PUBDEFのないE1'		;ERROR
;MK	TD_E1_PUBDEF_ERR,'E1 without PUBDEF'		;ERROR
MK	TD_SCOPE_NEST_ERR,'SCOPEが欠けている'		;ERROR
;MK	TD_SCOPE_NEST_ERR,'Missing SCOPE'		;ERROR
MK	TD_NAMES_COUNT_ERR,'名称数 > 64k (TD 3.xの制限)'	;ERROR
;MK	TD_NAMES_COUNT_ERR,'>64k Names, TD 3.x limit'	;ERROR
MK	TD_LOCALS_COUNT_ERR,'ローカル数 > 64k (TD 3.xの制限)'	;ERROR
;MK	TD_LOCALS_COUNT_ERR,'>64k Locals, TD 3.x limit'	;ERROR
MK	TD_TYPES_COUNT_ERR,'タイプ数 > 64k (TD 3.xの制限)';ERROR
;MK	TD_TYPES_COUNT_ERR,'>64k Types, TD 3.x limit'	;ERROR
MK	TD_MEMBERS_COUNT_ERR,'メンバー数 > 64k (TD 3.xの制限)'	;ERROR
;MK	TD_MEMBERS_COUNT_ERR,'>64k Members, TD 3.x limit'	;ERROR
MK	TD_LINES_COUNT_ERR,'行番号 > 64k (TD 3.xの制限)'	;ERROR
;MK	TD_LINES_COUNT_ERR,'>64k Linenumbers, TD 3.x limit'	;ERROR
MK	TD_SYMBOLS_COUNT_ERR,'シンボル数 > 64k (TD の制限)'	;FATAL
;MK	TD_SYMBOLS_COUNT_ERR,'>64k Symbols, TD limit'	;FATAL
;; ??? what is coverages?
MK	TD_COVERAGE_COUNT_ERR,'>64k Coverages, (TD の制限)';FATAL
;MK	TD_COVERAGE_COUNT_ERR,'>64k Coverages, TD limit';FATAL
MK	TD_TID_ERR,'不明TID。SLRに連絡願います。'		;FATAL
;MK	TD_TID_ERR,'Unknown TID, notify SLR'		;FATAL
MK	TD_CLASS_ERR,'Classレコードが解釈できない'	;FATAL
;MK	TD_CLASS_ERR,'Cannot parse Class record'	;FATAL
MK	TLINK_XTRA_ERR,'余計な文字を無視'	;warn
;MK	TLINK_XTRA_ERR,'Extra characters ignored'	;warn
if	debug
MK	TYPE_PUNT_ERR,'TYPE Punt'
MK	TYPE_PUNT_SUCCESS_ERR,'TYPE Success, No Punt'
MK	MEMBER_PUNT_ERR,'MEMBER Punt'
MK	CLASS_PUNT_ERR,'CLASS Punt'
MK	TD_UNUSED_TYPES_ERR,'Unused Types'
MK	TD_UNUSED_CLASSES_ERR,'Unused Classes'
MK	CLASS_ZERO_ERR,'Class Index == 0'
MK	FIXED_SPEC_ERR,'Fixed SpecFunc'
MK	TYPE_ZERO_ERR,'TYPE ZERO'
MK	CANNOT_FIX_SPEC_ERR,'Cannot Fix SpecialFunction'
endif

endif

if	debug
MK	DGROUP_STEPPED_ERR,'DGROUP Wedged'
MK	VIRT_FAIL_ERR,'Illegal Virtual Address'
endif

MK	INVALID_OBJ_ERR,'モジュールまたは辞書が破壊されている'	;fatal
;MK	INVALID_OBJ_ERR,'Module or Dictionary corrupt'	;fatal
MK	FATAL_RESTART_ERR,'致命的なRestartエラー'		;fatal
;MK	FATAL_RESTART_ERR,'FATAL Restart error'		;fatal
MK	FIX_LIB_ERR,'ライブラリにFIXLIBが必要な可能性がある'	;warning
;MK	FIX_LIB_ERR,'Library probably needs FIXLIB'	;warning
MK	NO_BASE_SEG_ERR,'未定義セグメントを参照している'	;fatal
;MK	NO_BASE_SEG_ERR,'References Undefined Segment'	;fatal

if	V5
MK	PAGESIZE_ERR,'/IMPLIBにより大きい/PAGESIZEが必要'	;fatal
;MK	PAGESIZE_ERR,'/IMPLIB needs larger /PAGESIZE'	;fatal
MK	DIR_TOO_BIG,'辞書が 512k を超えた'		;fatal
;MK	DIR_TOO_BIG,'Dictionary Exceeded 512k'		;fatal
if	fg_virt
MK	CANT_WINPACK_ERR,'REALモードではWINPACKが使えない'	;fatal
;MK	CANT_WINPACK_ERR,'Cannot WINPACK in REAL mode'	;fatal
endif
endif
MK	IMPORT_TOO_LONG_ERR,'名称によるインポートができない(>468文字)'	;fatal
;MK	IMPORT_TOO_LONG_ERR,'Cannot Import by Name >468 chars'	;fatal
MK	EXPORT_TOO_LONG_ERR,'名称によるエクスポートができない(>468文字)'	;fatal
;MK	EXPORT_TOO_LONG_ERR,'Cannot Export by Name >468 chars'	;fatal
MK	RES_TOO_LONG_ERR,'リソース名が >468文字'	;fatal
;MK	RES_TOO_LONG_ERR,'Resource Name >468 chars'	;fatal
MK	TOO_LONG_ERR,'シンボルが >468文字'		;fatal
;MK	TOO_LONG_ERR,'Symbol >468 chars'		;fatal

MK	USE32_USE16_ERR,'USE16/USE32の矛盾'		;error
;MK	USE32_USE16_ERR,'USE16/USE32 Mismatch'		;error
MK	LIDATA_ERR,'不正LIDATAレコード'		;fatal
;MK	LIDATA_ERR,'Illegal LIDATA Record'		;fatal
MK	STACK_NZERO_ERR,'スタックエラー'			;error
;MK	STACK_NZERO_ERR,'Stack Error'			;error

if	fg_pe
MK	RES_CONV_ERR,'16ビット→32ビットの.RES変換エラー'	;fatal
;MK	RES_CONV_ERR,'16-bit to 32-bit .RES conversion failed'	;fatal
MK	VERSION_BIG_ERR,'VERSIONリソースの16→32ビット変換が未サポート';fatal
;MK	VERSION_BIG_ERR,'Cannot yet convert VERSION resources from 16 to 32-bit';fatal
endif

MK	NO_IMPORTS_ERR,'Import Declarations Illegal'		;fatal
MK	NEAR_IMPORT_ERR,'NEAR call to IMPORT'
MK	CMDLINE_ERR,'Cmdline too long'
MK	MODEL_CONFLICT_ERR,'Memory Model Conflict'	;warning
MK	DUP_LOD_ERR,'Duplicate .LOD Specified'		;warning




		EVEN	?
ERR_TABLE	EQU	THIS WORD

LOOP_CNT	=	0
		REPT	ERR_CNT
		BUILD	%LOOP_CNT
LOOP_CNT	=	LOOP_CNT+1
		ENDM

OFFSET_ADR	DB	' オフセット '
;OFFSET_ADR	DB	' Offset '
ERROR_ADR	DB	' エラー '
;ERROR_ADR	DB	' Error '
WARN_ADR	DB	' 警告 '
;WARN_ADR	DB	' Warning '
RECTYP_ADR	DB	'レコードタイプ'
;RECTYP_ADR	DB	'Record Type '

if	fg_segm

PREVOFF_ADR	DB	'  前のオフセット: '
;PREVOFF_ADR	DB	'  Previous Offset: '
NAME_ADR	DB	'  名称: '
;NAME_ADR	DB	'  Name: '
TYPE_ADR	DB	0DH,0AH,'タイプ: '
;TYPE_ADR	DB	0DH,0AH,'Type: '

		EVEN	?

PRC_RC_TYPES	LABEL	WORD

		DW	RT_0000H		;0
		DW	RT_CURSOR		;1
		DW	RT_BITMAP		;2
		DW	RT_ICON			;3
		DW	RT_MENU			;4
		DW	RT_DIALOG		;5
		DW	RT_STRING		;6
		DW	RT_FONTDIR		;7
		DW	RT_FONT			;8
		DW	RT_ACCELERATORS		;9
		DW	RT_RCDATA		;10
		DW	RT_ERRTABLE		;11
		DW	RT_GROUP_CURSOR		;12
		DW	RT_000DH		;13
		DW	RT_GROUP_ICON		;14
		DW	RT_NAMETABLE		;15

RT_0000H	DB	LENGTH RT_0000H-1,'0000'
RT_CURSOR	DB	LENGTH RT_CURSOR-1,'カーソル'
;RT_CURSOR	DB	LENGTH RT_CURSOR-1,'Cursor'
RT_BITMAP	DB	LENGTH RT_BITMAP-1,'ビットマップ'
;RT_BITMAP	DB	LENGTH RT_BITMAP-1,'Bitmap'
RT_ICON		DB	LENGTH RT_ICON-1,'アイコン'
;RT_ICON		DB	LENGTH RT_ICON-1,'Icon'
RT_MENU		DB	LENGTH RT_MENU-1,'メニュー'
;RT_MENU		DB	LENGTH RT_MENU-1,'Menu'
RT_DIALOG	DB	LENGTH RT_DIALOG-1,'ダイアログ'
;RT_DIALOG	DB	LENGTH RT_DIALOG-1,'Dialog'
RT_STRING	DB	LENGTH RT_STRING-1,'ストリングテーブル'
;RT_STRING	DB	LENGTH RT_STRING-1,'String Table'
RT_FONTDIR	DB	LENGTH RT_FONTDIR-1,'フォントディレクトリ'
;RT_FONTDIR	DB	LENGTH RT_FONTDIR-1,'Font Directory'
RT_FONT		DB	LENGTH RT_FONT-1,'フォント'
;RT_FONT		DB	LENGTH RT_FONT-1,'Font'
RT_ACCELERATORS	DB	LENGTH RT_ACCELERATORS-1,'アクセラレータ'
;RT_ACCELERATORS	DB	LENGTH RT_ACCELERATORS-1,'Accelerator'
RT_RCDATA	DB	LENGTH RT_RCDATA-1,'RCDATA'
RT_ERRTABLE	DB	LENGTH RT_ERRTABLE-1,'エラーテーブル'
;RT_ERRTABLE	DB	LENGTH RT_ERRTABLE-1,'Error Table'
RT_GROUP_CURSOR	DB	LENGTH RT_GROUP_CURSOR-1,'グループカーソル'
;RT_GROUP_CURSOR	DB	LENGTH RT_GROUP_CURSOR-1,'Group Cursor'
RT_000DH	DB	LENGTH RT_000DH-1,'000D'
RT_GROUP_ICON	DB	LENGTH RT_GROUP_ICON-1,'グループアイコン'
;RT_GROUP_ICON	DB	LENGTH RT_GROUP_ICON-1,'Group Icon'
RT_NAMETABLE	DB	LENGTH RT_NAMETABLE-1,'名称テーブル'
;RT_NAMETABLE	DB	LENGTH RT_NAMETABLE-1,'Name Table'

endif

OPTLINK_TEXT	DB	'OPTLINK :'

XX	=	0

STD_HELP	MACRO	X

		PUBLIC	X&_EQU

X&_EQU		EQU	(XX)
XX		=	XX+1

		DW	X&_TEXT

		ENDM

		EVEN	?

STD_TEXT_TABLE	LABEL	WORD

		STD_HELP	LIBRARY
		STD_HELP	SEGCLASS
		STD_HELP	FILES
		STD_HELP	PUBSYMS
		STD_HELP	SEGMENTS
		STD_HELP	SEGMODS
		STD_HELP	GROUPS
		STD_HELP	MODULES
		STD_HELP	OUTFILES
		STD_HELP	AREAS
		STD_HELP	SECTIONS
		STD_HELP	CSEGS
		STD_HELP	RELOCS
		STD_HELP	SRCS
if	fg_td
		STD_HELP	TDLOCALS
		STD_HELP	TDCLASSES
		STD_HELP	TDLTYPES
		STD_HELP	TDLCLASSES
		STD_HELP	TDLMEMBERS
endif

if	fg_segm OR fg_pe
		STD_HELP	IMPNAMES
		STD_HELP	ENTNAMES
		STD_HELP	PENTS
		STD_HELP	ENTRIES
		STD_HELP	IMPMODS
		STD_HELP	RESNAMES
		STD_HELP	RESTYPES
		STD_HELP	RES_TYPENAMES
		STD_HELP	RTNLS
endif

if	fg_pe
		STD_HELP	PAGE_RELOCS
endif

LIBRARY_TEXT	DB	LENGTH LIBRARY_TEXT-1,'ライブラリファイル'
;LIBRARY_TEXT	DB	LENGTH LIBRARY_TEXT-1,'Library Files'
SEGCLASS_TEXT	DB	LENGTH SEGCLASS_TEXT-1,'セグメントクラス'
;SEGCLASS_TEXT	DB	LENGTH SEGCLASS_TEXT-1,'Segment Classes'
FILES_TEXT	DB	LENGTH FILES_TEXT-1,'ファイル名'
;FILES_TEXT	DB	LENGTH FILES_TEXT-1,'Filenames'
PUBSYMS_TEXT	DB	LENGTH PUBSYMS_TEXT-1,'グローバルシンボル'
;PUBSYMS_TEXT	DB	LENGTH PUBSYMS_TEXT-1,'Global Symbols'
SEGMENTS_TEXT	DB	LENGTH SEGMENTS_TEXT-1,'セグメント'
;SEGMENTS_TEXT	DB	LENGTH SEGMENTS_TEXT-1,'Segments'
SEGMODS_TEXT	DB	LENGTH SEGMODS_TEXT-1,'セグメント部分'
;SEGMODS_TEXT	DB	LENGTH SEGMODS_TEXT-1,'Segment Pieces'
GROUPS_TEXT	DB	LENGTH GROUPS_TEXT-1,'グループ'
;GROUPS_TEXT	DB	LENGTH GROUPS_TEXT-1,'Groups'
MODULES_TEXT	DB	LENGTH MODULES_TEXT-1,'モジュール'
;MODULES_TEXT	DB	LENGTH MODULES_TEXT-1,'Modules'
OUTFILES_TEXT	DB	LENGTH OUTFILES_TEXT-1,'出力ファイル名'
;OUTFILES_TEXT	DB	LENGTH OUTFILES_TEXT-1,'Output Filenames'
AREAS_TEXT	DB	LENGTH AREAS_TEXT-1,'領域(Area)'
;AREAS_TEXT	DB	LENGTH AREAS_TEXT-1,'Areas'
SECTIONS_TEXT	DB	LENGTH SECTIONS_TEXT-1,'セクション'
;SECTIONS_TEXT	DB	LENGTH SECTIONS_TEXT-1,'Sections'
CSEGS_TEXT	DB	LENGTH CSEGS_TEXT-1,'デバッグ用セグメント部分'
;CSEGS_TEXT	DB	LENGTH CSEGS_TEXT-1,'Debug Segment Pieces'
RELOCS_TEXT	DB	LENGTH RELOCS_TEXT-1,'再配置'
;RELOCS_TEXT	DB	LENGTH RELOCS_TEXT-1,'Relocations'
SRCS_TEXT	DB	LENGTH SRCS_TEXT-1,'ソースファイル'
;SRCS_TEXT	DB	LENGTH SRCS_TEXT-1,'Source Files'

if	fg_td
TDLOCALS_TEXT	DB	LENGTH	TDLOCALS_TEXT-1,'TDローカルシンボル'
;TDLOCALS_TEXT	DB	LENGTH	TDLOCALS_TEXT-1,'TD Local Symbols'
TDCLASSES_TEXT	DB	LENGTH	TDCLASSES_TEXT-1,'TDクラス'
;TDCLASSES_TEXT	DB	LENGTH	TDCLASSES_TEXT-1,'TD Classes'
TDLTYPES_TEXT	DB	LENGTH	TDLTYPES_TEXT-1,'TDローカル型'
;TDLTYPES_TEXT	DB	LENGTH	TDLTYPES_TEXT-1,'TD Local Types'
TDLCLASSES_TEXT	DB	LENGTH TDLCLASSES_TEXT-1,'TDローカルクラス'
;TDLCLASSES_TEXT	DB	LENGTH TDLCLASSES_TEXT-1,'TD Local Classes'
TDLMEMBERS_TEXT	DB	LENGTH TDLMEMBERS_TEXT-1,'TDローカルメンバ'
;TDLMEMBERS_TEXT	DB	LENGTH TDLMEMBERS_TEXT-1,'TD Local Members'
endif

if	fg_segm OR fg_pe
IMPNAMES_TEXT	DB	LENGTH IMPNAMES_TEXT-1,'インポート名'
;IMPNAMES_TEXT	DB	LENGTH IMPNAMES_TEXT-1,'Imported Names'
ENTNAMES_TEXT	DB	LENGTH ENTNAMES_TEXT-1,'エクスポート名'
;ENTNAMES_TEXT	DB	LENGTH ENTNAMES_TEXT-1,'Exported Names'
PENTS_TEXT	DB	LENGTH PENTS_TEXT-1,'Possible Entries'
;PENTS_TEXT	DB	LENGTH PENTS_TEXT-1,'Possible Entries'
ENTRIES_TEXT	DB	LENGTH ENTRIES_TEXT-1,'エクスポートシンボル'
;ENTRIES_TEXT	DB	LENGTH ENTRIES_TEXT-1,'Exported Symbols'
IMPMODS_TEXT	DB	LENGTH IMPMODS_TEXT-1,'インポートモジュール'
;IMPMODS_TEXT	DB	LENGTH IMPMODS_TEXT-1,'Imported Modules'
RESNAMES_TEXT	DB	LENGTH RESNAMES_TEXT-1,'RESの名称'
;RESNAMES_TEXT	DB	LENGTH RESNAMES_TEXT-1,'RES Names'
RESTYPES_TEXT	DB	LENGTH RESTYPES_TEXT-1,'RESのタイプ'
;RESTYPES_TEXT	DB	LENGTH RESTYPES_TEXT-1,'RES Types'
RES_TYPENAMES_TEXT	DB	LENGTH RES_TYPENAMES_TEXT-1,'RESの型名'
;RES_TYPENAMES_TEXT	DB	LENGTH RES_TYPENAMES_TEXT-1,'RES TypeNames'
RTNLS_TEXT	DB	LENGTH RTNLS_TEXT-1,'RESのRTNL'
;RTNLS_TEXT	DB	LENGTH RTNLS_TEXT-1,'RES RTNLs'

endif

if	fg_pe

PAGE_RELOCS_TEXT	DB	LENGTH PAGE_RELOCS_TEXT-1,'再配置ページ'
;PAGE_RELOCS_TEXT	DB	LENGTH PAGE_RELOCS_TEXT-1,'Relocation Pages'

endif

ERROR_DATA	ENDS

.DATA

EOF_TXT		DB	'[EOF]'


