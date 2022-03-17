#include <Arduino.h>
#include <Wire.h>

/*
 * 只能识别汉字，将要识别的汉字转换成拼音字母，每个汉字之间空格隔开，比如：小易同学 --> xiao yi tong xue
 * 最多添加50个词条，每个词条最长为79个字符，每个词条最多10个汉字
 * 每个词条都对应一个识别号（1~255随意设置）不同的语音词条可以对应同一个识别号，
 * 比如“易创空间”和“易创”都可以将识别号设置为同一个值
 * 模块上的STA蓝色状态灯：亮起表示正在识别语音，灭掉表示不会识别语音，当识别到语音时状态灯会变暗，或闪烁，等待读取后会恢复当前的状态指示
 */
#define VOICE_IIC_ADDR 0x79
#define VOICE_RESULT_REG 0    //识别结果存放寄存器地址地址，通过不断读取此地址的值判断是否识别到语音，不同的值对应不同的语音，
#define VOICE_ERASE_REG  0x1  //擦除所有词条寄存器地址
#define VOICE_MODE_REG   0x2  //设置模式寄存器地址


//识别模式设置，值范围1~4
//1：循环识别模式，任何语句直接识别    
//2：口令模式，以第一个词条为口令。当识别到口令词时常亮，等待识别到新的语音,并且读取识别结果后即灭掉
//3：按键模式，按下开始识别，不按不识别。支持掉电保存。状态灯随按键按下而亮起，不按不亮
//4：口令按键模式，当识别口令或者按键的时候都进入识别状态
#define VOICE_CONFIG_TIME_REG 0x3  // 配置口令等待时间 默认5s
#define VOICE_ADD_WORDS_REG 0x04 //词条添加的地址，支持掉电保存
#define VOICE_RESET_REG  0x5  //复位寄存器地址
#define VOICE_ASR_START_REG  0x6  //复位寄存器地址

typedef enum
{
    LOOP_MODE = 0,        // 循环模式
    BUTTON_MODE = 1,      // 按键模式
    KEYWORDS_MODE = 2,    // 关键字模式
    KEYWORDS_AND_BUTTON = 3, //关键字加按键模式
}E_WORK_MODE;

bool ld3320_add_words(unsigned char idNum, const char *words);
bool ld3320_config_time(unsigned char t);
bool ld3320_config_mode(E_WORK_MODE m);
bool ld3320_config_keywords(const char *words);
// bool ld3320_clear(void); // 擦除所有词条
bool ld3320_reset(void); // 复位语音识别模块
bool ld3320_asr_start(void); // 语音识别模块开始识别
unsigned char ld3320_get_result(void);
