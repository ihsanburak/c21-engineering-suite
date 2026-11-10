"""Interactive preview of the actual host C UI; not a TI-Nspire emulator."""
from pathlib import Path
import subprocess,sys,argparse,json
root=Path(__file__).resolve().parents[1]
parser=argparse.ArgumentParser(description=__doc__)
parser.add_argument('--engine',type=Path)
parser.add_argument('--smoke-test',action='store_true')
options=parser.parse_args()
if options.engine is None:
    candidates=list((root/'build').glob('host-preview-*.exe'))
    if not candidates:parser.error('Run tools/preview-ui.ps1 to build the current C preview first')
    options.engine=max(candidates,key=lambda p:p.stat().st_mtime_ns)
    newest_source=max(p.stat().st_mtime_ns for p in (root/'src').glob('*') if p.is_file())
    if options.engine.stat().st_mtime_ns<newest_source:parser.error('Preview binary is stale; run tools/preview-ui.ps1 again')
process=subprocess.Popen([str(options.engine)]+(["--temporary-settings","--trace"] if options.smoke_test else []),stdin=subprocess.PIPE,stdout=subprocess.PIPE,stderr=subprocess.PIPE if options.smoke_test else None)
preview_turkish=False
preview_ascii=False
preview_state={}
def frame(key=None):
    global preview_turkish,preview_ascii,preview_state
    if key is not None:
        process.stdin.write(f'{key}\n'.encode());process.stdin.flush()
    data=bytearray()
    while len(data)<76801:
        part=process.stdout.read(76801-len(data))
        if not part:raise RuntimeError('Host preview exited before producing a complete frame')
        data.extend(part)
    if options.smoke_test:preview_state=json.loads(process.stderr.readline())
    preview_turkish=bool(data[0]&2)
    preview_ascii=bool(data[0]&4)
    return data[0]&1,bytes(255 if v==0 else 0 for v in data[1:])
def close():
    if not process.stdin.closed:process.stdin.close()
    try:process.wait(timeout=2)
    except subprocess.TimeoutExpired:process.terminate();process.wait(timeout=2)
try:
    running,pixels=frame()
    if options.smoke_test:
        def keys(sequence):
            for key in sequence:frame(key)
        def root_menu():
            for _ in range(12):
                if preview_state['screen']==1 and preview_state['menu']==0:return
                running,_pixels=frame(6)
                assert running
            raise AssertionError('Could not restore root menu')
        assert running and len(pixels)==76800
        running,pixels=frame(5) # Enter splash.
        running,pixels=frame(32) # 0 directly opens category 10.
        running,pixels=frame(33) # 1: DC handbook group.
        running,pixels=frame(33) # 1: voltage formula.
        running,pixels=frame(4) # Definitions.
        running,pixels=frame(5) # Calculator.
        running,pixels=frame(17) # F: Formula Mode.
        running,pixels=frame(5) # Enter the selected current variable.
        running,pixels=frame(12) # Clear.
        running,pixels=frame(34) # Type 2.
        running,pixels=frame(5) # Commit.
        running,pixels=frame(4) # Next variable R.
        running,pixels=frame(5)
        running,pixels=frame(12)
        for key in (33,32,32):running,pixels=frame(key) # 100 ohms.
        running,pixels=frame(5)
        running,pixels=frame(18) # C: calculate.
        running,pixels=frame(20) # O: output selector.
        running,pixels=frame(34) # Choose current.
        running,pixels=frame(17) # Back to List Mode.
        assert running and 0 in pixels and 255 in pixels
        before_language=pixels
        for key in (23,33,34,6,6):running,pixels=frame(key) # Settings > Language > Turkce ASCII.
        assert pixels!=before_language and preview_ascii and preview_turkish
        ascii_turkish=pixels
        for key in (23,33,35,6,6):running,pixels=frame(key) # Retired third shortcut does nothing.
        assert pixels==ascii_turkish
        for key in (23,33,33,6,6):running,pixels=frame(key) # English restores original view.
        assert pixels==before_language
        # Enter two new calculators through the real four-input/unit UI.
        for key in (6,6,6,36,35,33,4,35,5,12,34,32,32,32,5,2,5,12,35,8,39,5,22,35,18):running,pixels=frame(key)
        assert running and 0 in pixels and 255 in pixels
        from PIL import Image
        Image.frombytes('L',(320,240),pixels).save(root/'build/phase3e1-pc-preview-battery.png')
        assert preview_state["calc"]==31 and preview_state["valid"] and abs(preview_state["result"]-26640)<1e-6
        root_menu()
        for key in (35,35,33):running,pixels=frame(key)
        for index,value in enumerate(('100','0.01','1E-6','1000')):
            running,pixels=frame(5);running,pixels=frame(12)
            for char in value:
                k=32+int(char) if char.isdigit() else {'.':8,'E':10,'-':9}[char]
                running,pixels=frame(k)
            running,pixels=frame(5)
            if index<3:running,pixels=frame(2)
        running,pixels=frame(18);running,pixels=frame(17)
        assert running and 0 in pixels and 255 in pixels
        Image.frombytes('L',(320,240),pixels).save(root/'build/phase3e1-pc-preview-rlc.png')
        assert preview_state['calc']==27 and preview_state['valid']
        # Phase 3E-2: new tools use the same Windows preview process and keypad transport.
        def keys(sequence):
            for key in sequence:frame(key)
        def fill(values):
            for i,value in enumerate(values):
                keys((5,12))
                for ch in value:frame(32+int(ch) if ch.isdigit() else {'.':8,'E':10,'-':9}[ch])
                frame(5)
                if i+1<len(values):frame(2)
        root_menu()
        keys((1+32,4+32,3+32)) # Formula RLC -> root -> EIRP.
        fill(('20','2','5'));running,pixels=frame(0)
        Image.frombytes('L',(320,240),pixels).save(root/'build/phase3e2-pc-preview-eirp-inputs.png')
        running,pixels=frame(18)
        assert preview_state['calc']==51 and preview_state['valid'] and abs(preview_state['result']-23)<1e-12
        Image.frombytes('L',(320,240),pixels).save(root/'build/phase3e2-pc-preview-eirp.png')
        root_menu()
        keys((1+32,5+32,2+32)) # Receiver noise in dBm.
        fill(('290','125000','6'));running,pixels=frame(18)
        assert preview_state['calc']==57 and preview_state['valid'] and abs(preview_state['result']+117.00608706414754)<1e-9
        Image.frombytes('L',(320,240),pixels).save(root/'build/phase3e2-pc-preview-noise-en.png')
        keys((23,33,34,6,6));running,pixels=frame(0)
        Image.frombytes('L',(320,240),pixels).save(root/'build/phase3e2-pc-preview-noise-tr.png')
        keys((11,39));running,pixels=frame(0) # Context Handbook example, Turkish.
        Image.frombytes('L',(320,240),pixels).save(root/'build/phase3e2-pc-preview-handbook-tr.png')
        keys((6,23,33,33,6,6))
        root_menu()
        keys((7+32,4+32,1+32)) # EN -> root -> full link budget.
        fill(('20','2','5','0','91.21817772541321','1'));running,pixels=frame(0)
        Image.frombytes('L',(320,240),pixels).save(root/'build/phase3e2-pc-preview-link-inputs.png')
        keys((1,1,1,1,1));running,pixels=frame(0)
        Image.frombytes('L',(320,240),pixels).save(root/'build/phase3e2-pc-preview-link-first-inputs.png')
        keys((2,2,2,2,2));running,pixels=frame(18)
        assert preview_state['calc']==68 and preview_state['valid'] and abs(preview_state['result']+69.21817772541321)<1e-9
        Image.frombytes('L',(320,240),pixels).save(root/'build/phase3e2-pc-preview-link-result.png')
        root_menu()
        keys((5+32,3+32,5+32)) # LP L-match.
        fill(('50','200','868E6'));keys((22,36));running,pixels=frame(18)
        assert preview_state['calc']==84 and preview_state['valid'] and abs(preview_state['result']-1.58792884626072e-8)<1e-18
        Image.frombytes('L',(320,240),pixels).save(root/'build/phase3e2-pc-preview-lmatch.png')
        assert running and 0 in pixels and 255 in pixels
        # Phase 3E-3: assert real current IDs/results, then capture native framebuffer.
        root_menu();keys((38,34,33))
        fill(('1.724E-8','0.1','0.001','35E-6'));running,pixels=frame(0)
        Image.frombytes('L',(320,240),pixels).save(root/'build/phase3e3-preview-trace-inputs.png')
        keys((1,1,1));running,pixels=frame(0)
        Image.frombytes('L',(320,240),pixels).save(root/'build/phase3e3-preview-trace-first-inputs.png')
        keys((2,2,2,22,34));running,pixels=frame(18)
        assert preview_state['calc']==88 and preview_state['valid'] and abs(preview_state['result']-.04925714285714286)<1e-12
        Image.frombytes('L',(320,240),pixels).save(root/'build/phase3e3-preview-trace-result.png')
        root_menu();keys((38,34,34));fill(('0.001','35E-6','10'));running,pixels=frame(18)
        assert preview_state['calc']==89 and preview_state['valid'] and preview_state['result']>0
        Image.frombytes('L',(320,240),pixels).save(root/'build/phase3e3-preview-ipc-warning.png')
        root_menu();keys((38,35,34)) # microstrip impedance
        fill(('0.0032','0.0016','4.3'));running,pixels=frame(18)
        assert preview_state['calc']==95 and preview_state['valid'] and abs(preview_state['result']-49.19180731824943)<1e-8
        Image.frombytes('L',(320,240),pixels).save(root/'build/phase3e3-preview-microstrip.png')
        root_menu();keys((40,35,33));fill(('0.2',));keys((22,34));running,pixels=frame(18)
        assert preview_state['calc']==97 and preview_state['valid'] and abs(preview_state['result']-.04)<1e-12
        Image.frombytes('L',(320,240),pixels).save(root/'build/phase3e3-preview-reflected.png')
        root_menu();keys((40,36,33));fill(('-3',));running,pixels=frame(18)
        assert preview_state['calc']==101 and preview_state['valid'] and preview_state['result']==3
        Image.frombytes('L',(320,240),pixels).save(root/'build/phase3e3-preview-s21.png')
        root_menu();keys((41,36,33));fill(('1000','0.05'));running,pixels=frame(18)
        assert preview_state['calc']==109 and preview_state['valid'] and abs(preview_state['result']-950)<1e-9
        Image.frombytes('L',(320,240),pixels).save(root/'build/phase3e3-preview-tolerance-en.png')
        keys((23,33,34,6,6));running,pixels=frame(0)
        Image.frombytes('L',(320,240),pixels).save(root/'build/phase3e3-preview-tolerance-tr.png')
        keys((11,39));running,pixels=frame(0)
        Image.frombytes('L',(320,240),pixels).save(root/'build/phase3e3-preview-handbook-tr.png')
        keys((6,23,33,33,6,6));root_menu();keys((40,37,37)) # sample SD, data item 5
        fill(('1','2','3','4'));running,pixels=frame(18)
        assert preview_state['calc']==120 and preview_state['valid'] and abs(preview_state['result']-1.2909944487358056)<1e-10
        Image.frombytes('L',(320,240),pixels).save(root/'build/phase3e3-preview-sample-sd.png')
        root_menu();keys((40,37,32));fill(('0.00123456','3'));running,pixels=frame(18)
        assert preview_state['calc']==125 and preview_state['valid'] and abs(preview_state['result']-.00123)<1e-14
        Image.frombytes('L',(320,240),pixels).save(root/'build/phase3e3-preview-format.png')
        # Phase 3F: full eight-parameter LoRa packet, flags, invalid LDRO and corrected result.
        root_menu();keys((33,39,33));fill(('7','125000','1','20','0','1','8','0'));running,pixels=frame(0)
        Image.frombytes('L',(320,240),pixels).save(root/'build/phase3f-preview-packet-last-fields-en.png')
        keys((1,1,1,1,1,1,1));running,pixels=frame(0)
        Image.frombytes('L',(320,240),pixels).save(root/'build/phase3f-preview-packet-first-fields-en.png')
        keys((2,2,2,2));running,pixels=frame(0)
        Image.frombytes('L',(320,240),pixels).save(root/'build/phase3f-preview-packet-middle-fields-en.png')
        keys((22,34));running,pixels=frame(18)
        assert preview_state['calc']==131 and preview_state['valid'] and abs(preview_state['result']-.056576)<1e-12
        Image.frombytes('L',(320,240),pixels).save(root/'build/phase3f-preview-packet-result-en.png')
        keys((23,33,34,6,6));running,pixels=frame(0)
        Image.frombytes('L',(320,240),pixels).save(root/'build/phase3f-preview-packet-result-tr.png')
        keys((11,33));running,pixels=frame(0)
        Image.frombytes('L',(320,240),pixels).save(root/'build/phase3f-preview-handbook-formula-tr.png')
        keys((38,2,2,2,2,2));running,pixels=frame(0)
        Image.frombytes('L',(320,240),pixels).save(root/'build/phase3f-preview-handbook-limits-tr.png')
        keys((6,23,33,34,6,6));running,pixels=frame(0)
        assert preview_turkish and preview_ascii and preview_state['valid']
        Image.frombytes('L',(320,240),pixels).save(root/'build/phase3f-preview-packet-result-tr-ascii.png')
        keys((23,33,33,6,6,23));running,pixels=frame(0)
        Image.frombytes('L',(320,240),pixels).save(root/'build/phase3f-preview-version-settings.png')
        keys((6,));root_menu();keys((33,39,33));fill(('12','125000','1','20','0','1','8','0'));running,pixels=frame(18)
        assert preview_state['calc']==131 and not preview_state['valid']
        Image.frombytes('L',(320,240),pixels).save(root/'build/phase3f-preview-invalid-ldro.png')
        keys((5,12,33,5));running,pixels=frame(18)
        assert preview_state['valid'] and abs(preview_state['result']-1.318912)<1e-12
        Image.frombytes('L',(320,240),pixels).save(root/'build/phase3f-preview-corrected-sf12.png')
        root_menu();keys((33,40,34));fill(('0.056576','3600','0.01'));running,pixels=frame(18)
        assert preview_state['calc']==136 and preview_state['valid'] and preview_state['result']==636
        Image.frombytes('L',(320,240),pixels).save(root/'build/phase3f-preview-budget.png')
        root_menu();keys((33,40,35));fill(('0.165','0.056576'));keys((22,34));running,pixels=frame(18)
        assert preview_state['calc']==137 and preview_state['valid'] and abs(preview_state['result']-.00933504)<1e-12
        Image.frombytes('L',(320,240),pixels).save(root/'build/phase3f-preview-energy.png')
        print('PASS: host preview transport, handbook, formula editing, outputs, modes and two-choice EN/Turkce ASCII switching, battery energy, RLC, RF EIRP/noise, six-input link budget and L-match, PCB resistance/microstrip, VNA power, tolerance, SD and SI formatting; eight-field LoRa, LDRO errors/correction, Turkce ASCII, application version, budget and energy (temporary settings)')
    else:
        import tkinter as tk
        from PIL import Image,ImageTk
        window=tk.Tk();window.title('C21 Phase 3E-1 — TR/EN Modern Mono preview (NOT hardware emulation)')
        display=tk.Label(window);display.pack()
        help_label=tk.Label(window);help_label.pack()
        def show(data):
            window.title('C21 Phase 3E-1 — TR/EN Modern Mono '+('önizleme (donanım emülasyonu değil)' if preview_turkish else 'preview (NOT hardware emulation)'))
            help_label.configure(text=('S Ayarlar • F Formül/Liste • O Çıkış • C Hesapla • U giriş / Shift-U çıkış birimi • H Bilgi\nGiriş: ←→ imleç • Home/End • Ctrl+Backspace temizle • Delete ileri sil • E üs' if preview_turkish else 'S Settings • F Formula/List • O Output • C Calculate • U input units / Shift-U output units • H Info\nInput: ←→ cursor • Home/End • Ctrl+Backspace clear • Delete forward • E exponent'))
            if preview_ascii:
                table=str.maketrans('ÇĞİÖŞÜçğıöşü','CGIOSUcgiosu')
                help_label.configure(text=help_label.cget('text').translate(table))
                window.title(window.title().translate(table))
            image=Image.frombytes('L',(320,240),data).resize((960,720),Image.Resampling.NEAREST)
            display.image=ImageTk.PhotoImage(image);display.configure(image=display.image)
        show(pixels)
        keys={'Up':1,'Down':2,'Left':3,'Right':4,'Return':5,'Escape':6,'BackSpace':7,'Delete':15,'period':8,'minus':9,'e':10,'E':10,'h':11,'H':11,'question':11,'Home':13,'End':14,'plus':16,'f':17,'F':17,'c':18,'C':18,'u':19,'U':22,'o':20,'O':20,'s':23,'S':23,'Tab':21}
        def press(event):
            key=keys.get(event.keysym)
            if event.keysym=='BackSpace' and event.state&4:key=12
            if event.char and event.char in '0123456789':key=32+int(event.char)
            if key is None:return
            try:
                active,data=frame(key)
                if active:show(data)
                else:window.destroy()
            except (BrokenPipeError,RuntimeError):window.destroy()
        window.bind('<KeyPress>',press);window.mainloop()
finally:close()
