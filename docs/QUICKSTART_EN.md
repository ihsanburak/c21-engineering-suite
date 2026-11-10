# Quick start — English

**C21 Engineering Suite 1.0 Classic Edition**, application build **1.0.2**.

Original monochrome TI-Nspire Touchpad, non-CAS, OS 3.9.0.463, existing compatible Ndless. See [installation](INSTALL.md); do not update firmware or reinstall Ndless.

## Controls

- Digits select numbered entries; **0 selects item 10**. Arrows navigate. Keyboard Enter or Touchpad center-click confirms; Esc goes back.
- **C** calculates. **F** switches Formula/List where supported. **O** chooses supported alternative outputs.
- Formula Mode: left/right/Tab focuses a variable, Enter edits, **U** selects input units, **Shift-U** output units.
- List Mode: up/down focuses fields/actions, Enter edits or activates; left/right on an input selects units.
- **H** opens the linked Handbook. Returning restores inputs and navigation state.
- Numeric input accepts decimal values, valid negative signs and scientific E notation. EE/e^x enters E. Del erases, Ctrl+Del clears, Ctrl+left/right moves to either end, Shift+Del forward-deletes. Enter commits; Esc cancels. Calculate again after an edit.
- **S** opens Settings → Language: **1. English**, **2. Turkce ASCII**. Values, units, mode and valid results are retained.

## Examples

**Ohm's law:** Basic Electronics → DC circuits → Ohm: current. Enter V=12 V and R=100 Ω. Calculate **0.12 A**. Change R to 200 Ω and calculate **0.06 A**.

**LC design:** RLC & Impedance → Resonance → Required capacitance. Enter f=868 MHz and L=10 nH; select pF output. Calculate about **3.362024 pF**. L=20 nH gives about **1.681012 pF**.

**RF power:** RF Calculator → RF power conversions → mW to dBm. Enter **100 mW** → **20 dBm**; 200 mW → approximately **23.0103 dBm**.

**LoRa:** RF Calculator → LoRa packet timing → Packet airtime. SF7, BW125 kHz, CR index1, physical PL20 bytes, IH0 (explicit), CRC1, preamble8, DE0 → **56.576 ms**. SF12 requires DE1; it gives **1.318912 s**. Use the configured modem flags, not assumed defaults.

Choose engineering units explicitly; internal calculations use SI. Check H for measurement instruments, symbol meanings, assumptions, worked examples and common mistakes. Handbook examples are reference material; live forms use your entered values.

## Limits and device evidence

LoRa is scoped to legacy SX127x SF7–12, BW125/250/500 kHz. Microstrip uses zero thickness; IPC current estimates are advisory. Statistics use four normalized readings; SMD codes are numeric-only. See [known limitations](KNOWN_LIMITATIONS.md).

The owner reports physical launch and passed LC/Ohm/RF-power examples. Final V1.0.2 center-click/language acceptance is pending; this is not full device or engineering certification.
