"""Compose approved masters; never create/trace/recolor a replacement Eagle.
Requires Pillow; optional Inkscape exports the composed SVGs to PNG.
"""
from pathlib import Path
from PIL import Image
import argparse,base64,hashlib,json,html,subprocess
p=argparse.ArgumentParser()
p.add_argument('--logo-png',required=True,type=Path,help='Owner-supplied transparent PNG')
p.add_argument('--logo-svg',type=Path,help='Optional actual official vector master, copied unchanged')
p.add_argument('--output',required=True,type=Path)
p.add_argument('--font-family',default='sans-serif',help='Owner-approved site/brand typography; no font file is embedded')
p.add_argument('--inkscape',help='Existing Inkscape executable for PNG composition exports')
a=p.parse_args()
im=Image.open(a.logo_png).convert('RGBA')
assert im.getextrema()[3][0]<255,'Opaque input: supply an approved transparent master; background removal is not automatic'
master=a.logo_png.read_bytes()
assert not any(x in master for x in [b'C:\\Users\\',b'/Users/']), 'Review identifying image metadata first'
files={'c21-eagle-original.png':master}
src=master;mime='image/png'
if a.logo_svg:
 src=a.logo_svg.read_bytes();s=src.decode('utf8')
 assert '<svg' in s and not any(x in s.lower() for x in ['<script','file://','c:\\users\\','/users/']), 'Review SVG scripts/metadata/external references first'
 files['c21-eagle-original.svg']=src;mime='image/svg+xml'
uri='data:'+mime+';base64,'+base64.b64encode(src).decode()
layouts={'github-banner':(1280,320),'readme-header':(1000,260),'release-artwork':(1600,900),'social-preview':(1280,640)}
for name,(w,h) in layouts.items():
 # Image is fitted by the renderer with uniform scale; source colors and shape are untouched.
 lx,lw,lh=64,min(220,w//5),min(h-96,240)
 ly=(h-lh)//2
 tx=lx+lw+48;ty=h//2-22
 font=html.escape(a.font_family,quote=True)
 svg=f'''<svg xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" width="{w}" height="{h}" viewBox="0 0 {w} {h}">
 <title>C21 Engineering Suite — 1.0 Classic Edition</title>
 <desc>Original official Eagle master embedded unchanged; neutral composition pending owner brand review. No font files embedded.</desc>
 <rect width="{w}" height="{h}" fill="white"/>
 <image x="{lx}" y="{ly}" width="{lw}" height="{lh}" preserveAspectRatio="xMidYMid meet" xlink:href="{uri}"/>
 <g fill="#111" font-family="{font}">
 <text x="{tx}" y="{ty-34}" font-size="19" letter-spacing="3">C21 LABS</text>
 <text x="{tx}" y="{ty+10}" font-size="32" font-weight="700">C21 Engineering Suite</text>
 <text x="{tx}" y="{ty+48}" font-size="24">1.0 Classic Edition</text>
 <text x="{tx}" y="{ty+84}" font-size="20">Engineering knowledge. Anywhere. Offline.</text>
 </g></svg>'''
 files[name+'.svg']=svg.encode('utf8')
 # A scaled transparent logo uses the original pixels/aspect; no color substitution.
import io
for width in [128,256,512]:
 size=(width,max(1,round(width*im.height/im.width)))
 out=io.BytesIO();im.resize(size,Image.Resampling.LANCZOS).save(out,format='PNG');files[f'c21-eagle-{width}.png']=out.getvalue()
files['masters.json']=(json.dumps({'png_sha256':hashlib.sha256(master).hexdigest(),'svg_sha256':hashlib.sha256(src).hexdigest() if a.logo_svg else None,'logo_shape':'unaltered, uniform fit','color_policy':'original source colors','fonts':'system references only; owner selects typography','rights':'requires owner confirmation'},indent=2)+'\n').encode()
for n,b in files.items():
 path=a.output/n;assert not path.exists() or path.read_bytes()==b,f'Refusing overwrite: {path}'
for n,b in files.items():
 path=a.output/n;path.parent.mkdir(parents=True,exist_ok=True)
 if not path.exists():path.write_bytes(b)
if a.inkscape:
 for name in layouts:
  dst=a.output/(name+'.png');assert not dst.exists(),f'Refusing overwrite: {dst}'
  subprocess.run([a.inkscape,str(a.output/(name+'.svg')),'--export-type=png','--export-filename='+str(dst)],check=True)
print('Prepared original-master compositions; review proportions, text fit, fonts and colors before approval. Raster-only masters produce raster-in-SVG layouts, not a vector Eagle.')
