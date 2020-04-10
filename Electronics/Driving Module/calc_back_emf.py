#!/usr/bin/env python3

from quantiphy import Quantity as Q

def calc(RMotors, RGnd, Voltage):
	print(f"Resistors to Motors: {Q(RMotors, 'Ω')} Resistor to Ground: {Q(RGnd, 'Ω')}")
	print(f"Voltage of Power Rail: {Q(Voltage, 'V')}")
	PDown = 1/(1/RMotors+1/RGnd)
	PUp = RMotors
	print(f"Pulling down: {Q(PDown, 'Ω')} Pulling up: {Q(PUp, 'Ω')}")
	Curr = Voltage/(PDown+PUp)
	print(f"Current: {Q(Curr, 'A')}")
	ADCVolt = Curr*PDown
	print(f"Voltage at ADC at full speed: {Q(ADCVolt, 'V')}")

calc(10e3, 3.3e3, 16.8)
