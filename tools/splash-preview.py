"""View actual 320x240 C animation frames. NOT Firebird/hardware emulation."""
from pathlib import Path
import subprocess,argparse,time,json
from PIL import Image,ImageTk
r=Path(__file__).resolve().parents[1]
p=argparse.ArgumentParser(description=__doc__);p.add_argument('--engine',required=True,type=Path);p.add_argument('--export-only',action='store_true');a=p.parse_args()
frames=56
start=time.perf_counter()
raw=subprocess.run([str(a.engine)],input=('\n'.join(map(str,range(frames)))+'\n4294967295\n').encode(),stdout=subprocess.PIPE,check=True).stdout
assert len(raw)==(frames+1)*76800
images=[Image.frombytes('L',(320,240),raw[i*76800:(i+1)*76800]).point([255,0]+[0]*254) for i in range(frames+1)]
folder=r/'build';folder.mkdir(exist_ok=True)
durations=[50]*frames;durations[-1]=600
images[0].save(folder/'splash-animation-320x240.gif',save_all=True,append_images=images[1:frames],duration=durations,disposal=2,optimize=False)
scaled=[im.resize((960,720),Image.Resampling.NEAREST) for im in images[:frames]]
scaled[0].save(folder/'splash-animation-3x.gif',save_all=True,append_images=scaled[1:],duration=durations,disposal=2,optimize=False)
for f in [0,12,25,34,44,55]:images[f].save(folder/f'splash-frame-{f:02}.png')
(folder/'splash-preview-timing.json').write_text(json.dumps({'source':'actual C renderer','frames':frames,'framebuffer':'320x240 black/white','host_render_and_export_seconds':time.perf_counter()-start,'native_frame_delay_ms':50,'hardware_timing_measured':False,'emulator_tested':False,'holds_final_until_input':True},indent=2)+'\n')
if a.export_only:print('PASS: C framebuffer animation GIFs and native-resolution frames exported; NOT emulator verification');raise SystemExit
import tkinter as tk
window=tk.Tk();window.title('C21 V1.0.3 ANIMATION TEST - PC preview / NOT Firebird')
label=tk.Label(window,bg='white');label.pack(padx=8,pady=8)
status=tk.Label(window,text='Actual C renderer - 320 x 240; 3x nearest-neighbor display');status.pack()
controls=tk.Frame(window);controls.pack(pady=8)
photos=[ImageTk.PhotoImage(im.resize((960,720),Image.Resampling.NEAREST)) for im in images]
state={'frame':0,'paused':False,'callback':None}
def show(f):label.configure(image=photos[f]);status.configure(text=f'PC preview - NOT hardware emulation | frame {f+1}/{frames} | Enter: skip | R: replay | Space: pause')
def cancel():
 if state['callback'] is not None:window.after_cancel(state['callback']);state['callback']=None
def tick():
 state['callback']=None
 if state['paused']:return
 f=state['frame'];show(f)
 if f<frames-1:state['frame']=f+1;state['callback']=window.after(50,tick)
 else:state['paused']=True;status.configure(text='Logo stays until input / Logo ekranda kalir | Enter / Gec: menu | R: replay. NOT Firebird.')
def replay(event=None):cancel();state.update(frame=0,paused=False);tick()
def skip(event=None):
 cancel();state['paused']=True;show(frames);status.configure(text='Main menu from actual C renderer | R / Replay to watch again. NOT Firebird emulation.')
def pause(event=None):
 state['paused']=not state['paused']
 if state['paused']:cancel()
 else:tick()
tk.Button(controls,text='Replay / Tekrar',command=replay).pack(side='left',padx=8)
tk.Button(controls,text='Pause / Duraklat',command=pause).pack(side='left',padx=8)
tk.Button(controls,text='Skip / Gec',command=skip).pack(side='left',padx=8)
window.bind('<Return>',skip);window.bind('<r>',replay);window.bind('<space>',pause)
replay();window.mainloop()
