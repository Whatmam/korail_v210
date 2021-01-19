//	@(#)rfio.h	1.0g	2015/08/12	(SOLIDTEK)

#define	READER_NAME		"RC531"
#define	READER_NAME_VERBOSE	"RC531 reader driver"

/* device numbers */
#define	READER_MAJOR		(242)

// rc531 장치를 ioctl 명령어로 접근시의 명령어 
#define RC531_IOC_INT_EN	1000	// RC531 인터럽트 인에이블
#define RC531_IOC_INT_DI	1100	// RC531 인터럽트 디스에이블
#define RC531_IOC_FLG_WR	2000	// 인터럽트 발생 플래그 값 설정
#define RC531_IOC_FLG_RD	2100 	// 인터럽트 발생 플래그 값 응답
#define RC531_IOC_RAW_WR	3000	// RC531 내부 Raw 레지스터 쓰기 
#define RC531_IOC_RAW_RD	3100	// RC531 내부 Raw 레지스터 읽기
#define RC531_IOC_REG_WR	4000	// RC531 내부 레지스터 쓰기
#define RC531_IOC_REG_RD	4100	// RC531 내부 레지스터 읽기
#define RC531_IOC_CLR_BV	5000	// RC531 내부 레지스터 bit clear
#define RC531_IOC_SET_BV	5100	// RC531 내부 레지스터 bit set
#define RC531_IOC_RST_HI	6000	// RC531 리셋핀 High 제어
#define RC531_IOC_RST_LO	6100	// RC531 리셋핀 Low 제어

// rc531 장치를 read/write 명령어로 접근시의 명령어 
#define RC531_WRD_FLG_WR	'A'
#define RC531_WRD_FLG_RD	'B'
#define RC531_WRD_RAW_WR	'C'
#define RC531_WRD_RAW_RD	'D'
#define RC531_WRD_REG_WR	'E'
#define RC531_WRD_REG_RD	'F'
#define RC531_WRD_CLR_BV	'G'
#define RC531_WRD_SET_BV	'H'

typedef unsigned char  byte;
typedef unsigned short word;
typedef unsigned int   dword;


