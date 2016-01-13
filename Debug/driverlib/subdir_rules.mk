################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
driverlib/%.o: ../driverlib/%.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: GNU Compiler'
	"C:/ti/ccsv6/tools/compiler/gcc-arm-none-eabi-4_8-2014q3/bin/arm-none-eabi-gcc.exe" -c -mcpu=cortex-m4 -march=armv7e-m -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -DPART_TM4C123GH6PM -DTARGET_IS_BLIZZARD_RB1 -Dgcc -I"C:/ti/ccsv6/tools/compiler/gcc-arm-none-eabi-4_8-2014q3/arm-none-eabi/include" -I"F:/Dropbox/Profissional/Konatus/Atividades/RTOS/Codigos/ex_BRTOS_Tiva/USB/cdc" -I"F:/Dropbox/Profissional/Konatus/Atividades/RTOS/Codigos/ex_BRTOS_Tiva/USB" -I"F:/Dropbox/Profissional/Konatus/Atividades/RTOS/Codigos/ex_BRTOS_Tiva/brtos" -I"C:/ti/TivaWare_C_Series-2.1.0.12573" -I"F:/Dropbox/Profissional/Konatus/Atividades/RTOS/Codigos/ex_BRTOS_Tiva" -I"F:/Dropbox/Profissional/Konatus/Atividades/RTOS/Codigos/ex_BRTOS_Tiva/hal" -I"F:/Dropbox/Profissional/Konatus/Atividades/RTOS/Codigos/ex_BRTOS_Tiva/brtos/includes" -ffunction-sections -fdata-sections -g -gdwarf-3 -gstrict-dwarf -Wall -MD -std=c99 -pedantic -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


