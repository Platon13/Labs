//#include <cstdint>            //for int types such as uint32_t
#include "gpiocregisters.hpp" //for Gpioc
#include "gpioaregisters.hpp" //for Gpioa
#include "rccregisters.hpp"   //for RCC
#include "tim2registers.hpp"   //for SPI2
#include "nvicregisters.hpp"  //for NVIC
#include "tim3registers.hpp"
#include "usart2registers.hpp" // for USART2
#include <string>
#include "MessageTransmitter.h"

using namespace std;

constexpr auto SystemClock = 16'000'000U;
constexpr auto TimerClock = 1'000U;
constexpr auto TimerPrescaler = SystemClock/TimerClock;
/*constexpr auto TimerPrescaler = 16000U;*/


extern "C"
{
int __low_level_init(void)
{
    //Switch on external 8 MHz oscillator
    RCC::CR::HSEON::On::Set() ;
    while (!RCC::CR::HSERDY::Ready::IsSet())
    {
    }
    //Switch system clock on external oscillator
    RCC::CFGR::SW::Hse::Set() ;
    while (!RCC::CFGR::SWS::Hsi::IsSet())
    {
    }
    
    RCC::AHB1ENR::GPIOCEN::Enable::Set(); //Подали тактирование на порт GPIOC
    GPIOC::MODER::MODER8::Output::Set();  //Настроили порт PORTC.8 на выход
    GPIOC::MODER::MODER5::Output::Set();  //Настроили порт PORTC.5 на выход
 //   GPIOC::MODER::MODER9::Output::Set();  //Настроили порт PORTC.9 на выход


 
    
 // Timer 2   

    RCC::APB1ENR::TIM2EN::Enable::Set();
    TIM2::PSC::Write(TimerPrescaler);
    TIM2::ARR::Write(1000);
    TIM2::CNT::Write(0);
    
    NVIC::ISER0::Write(1 << 28U);
    TIM2::DIER::UIE::Enable::Set();
    TIM2::CR1::CEN::Enable::Set();

 //Timer 3
    
    RCC::APB1ENR::TIM3EN::Enable::Set();
    TIM3::PSC::Write(TimerPrescaler);
    TIM3::ARR::Write(500);
    TIM3::CNT::Write(0);
    
    NVIC::ISER0::Write(1 << 29U);
    TIM3::DIER::UIE::Enable::Set( );
    TIM3::CR1::CEN::Enable::Set( );
   
 
//port A k sisteme tactirovaniy

    RCC::AHB1ENR::GPIOAEN::Enable::Set();    
    
//port A2 i A3 na alternativn rezhim raboty

    GPIOA::MODER::MODER2::Alternate::Set();
    GPIOA::MODER::MODER3::Alternate::Set();

//port A2 i A3 na alternativn fynkciu
    
    GPIOA::AFRL::AFRL2::Af7::Set();  // USART2 Tx
    GPIOA::AFRL::AFRL3::Af7::Set();  // USART2 Rx    
    
// podklychenie USART2 k sisteme taktirovania APB1
    RCC::APB1ENR::USART2EN::Enable::Set();

    USART2::CR1::OVER8::OversamplingBy16::Set();
    USART2::CR1::M::Data8bits::Set();
    USART2::CR1::PCE::ParityControlDisable::Set();
    
    USART2::BRR::Write(8'000'000 / 9600); // 8 MHz
    
    NVIC::ISER1::Write(1 << 6U);
    
  return 1;
}
}

void DelayMs (uint32_t value)
{
  const auto delay = (value * TimerClock/ 1000U);
  TIM2::PSC::Write(TimerPrescaler);
  TIM2::ARR::Write(11000);
  TIM2::SR::UIF::NoUpdate::Set();
  TIM2::CNT::Write(0U);
  

while(TIM2::SR::UIF::NoUpdate::IsSet())
  {
  }
    TIM2::SR::UIF::NoUpdate::Set();
    TIM2::CR1::CEN::Disable::Set();
  
}

int main(void)
{
  std::string TestMessage = "Hello world! ";

  for(;;)
  {
    MessageTransmitter::Send(TestMessage);
  }
}


