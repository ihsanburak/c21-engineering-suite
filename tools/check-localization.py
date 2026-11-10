"""Audit all current UI/engine/handbook prose keys and both languages' bitmap coverage."""
from pathlib import Path
import re,json
r=Path(__file__).resolve().parents[1]
catalog=json.loads((r/'locales/tr.json').read_text(encoding='utf-8'))
def strings(path):return [json.loads('"'+s+'"') for s in re.findall(r'"((?:[^"\\]|\\.)*)"',(r/path).read_text(encoding='utf-8'))]
ui=strings('src/ui.c')
invariant={'N = %.0f pkt','%.*g E%+d(%s)','ε_e = %.6g; Z0 = %.6g Ω','%.*E','%.*g %s','%.*g E%+d','p','n','μ','m','k','M','G','T','C','L','Γ = %.6g %+.6gj','pF','nH','Z = %.6g %+.6gj Ω','Turkce (ASCII)','3. Turkce (ASCII)','1. Türkçe (TR)','2. English (EN)','Türkçe (TR)','English (EN)','\n%s: 1 %s = %.8g SI'}
# Language self-names are deliberately invariant in both display languages.
invariant.update({"English","Turkce ASCII","1. English","2. Turkce ASCII"})
required=set()
for s in ui:
 if s.endswith('.h') or s in invariant:continue
 stripped=re.sub(r'%[.0-9]*[A-Za-z]','',s)
 if re.search('[A-Za-z]',stripped):required.add(s)
engine=(r/'src/engine.c').read_text(encoding='utf-8')
metadata=re.findall(r'^ \{"([^"\n]*)","([^"\n]*)","([^"\n]*)","([^"\n]*)"',engine,re.M)
for name,formula,defs,usage in metadata:required.update((name,defs,usage))
book=strings('src/handbook.c')[1:];assert len(book)==6*len(metadata);required.update(book)
meaning=re.search(r'names\[VAR_COUNT\]=\{([^}]+)',engine).group(1)
required.update(re.findall(r'"([^"]+)"',meaning));required.add('Constant')
required.update(s for s in strings('src/numeric_input.c') if s and not s.endswith('.h'))
required.update(('OK','Invalid input or passive-domain violation','Numeric overflow/underflow'))
missing=sorted(required-catalog.keys());assert not missing,missing
font={int(n) for n in re.findall(r'\{(\d+),\{',(r/'src/font_data.h').read_text())}
unsupported={c for pair in catalog.items() for s in pair for c in s if c!='\n' and ord(c) not in font};assert not unsupported,unsupported
for c in 'ÇĞİÖŞÜçğıöşü':assert ord(c) in font
# Explicit dynamic presentation boundaries, not merely presence in the catalog.
source=(r/'src/ui.c').read_text(encoding='utf-8')
for boundary in ('tr(c->name)','tr(c->definitions)','tr(h->purpose)','tr(h->measurements)','tr(h->laboratory)','tr(h->limits)','tr(h->mistakes)','tr(m->entries[i].name)','tr(titles[a->info_page])','tr(variable_meaning(id))','tr(numeric_error_text(a->input_error))','tr(status_text(a->error))'):
 assert boundary in source,boundary
# Check the actual assembled UTF-8 body against the fixed UI buffer.
units={n:[(s,float(v)) for _,s,v in re.findall(r'\{"([^"]*)","([^"]*)",([^}]+)\}',b)] for n,b in re.findall(r'static const Unit (\w+)\[\]=\{(.*?)\};',engine)}
extra='\nInput SI = number × unit multiplier.\nOutput = SI / unit multiplier.\nChoose each unit in the calculator.'
lengths=[]
for language in ('EN','TR'):
 for i,row in enumerate(re.findall(r'^ \{".*$',engine,re.M)):
  body=(catalog[book[i*6+1]] if language=='TR' else book[i*6+1])+(catalog[extra] if language=='TR' else extra)
  for symbol,unit in re.findall(r'P\("([^"]+)",(\w+)\)',row):
   for label,scale in units[unit]:
    if symbol=='dBm':
     s='\ndBm: logarithmic input, no SI prefix.';body+=catalog[s] if language=='TR' else s
    else:body+=f'\n{symbol}: 1 {label} = {scale:.8g} SI'
  lengths.append(len(body.encode('utf-8')))
assert max(lengths)<1024
report={'max_measurements_body_utf8_bytes':max(lengths),'turkish_string_bytes':sum(len(v.encode('utf-8'))+1 for v in catalog.values()),'catalog_translations':len(catalog),'required_current_keys':len(required),'missing_keys':missing,'unsupported_glyphs':[], 'handbook_entries':len(metadata),'handbook_prose_fields':len(book),'required_turkish_glyphs':12,'invariant_ui_strings':sorted(invariant)}
(r/'build/phase3d-localization-audit.json').write_text(json.dumps(report,ensure_ascii=False,indent=2)+'\n',encoding='utf-8')
print(f'PASS: {len(required)} required current text keys, {len(catalog)} translations, {len(book)} Handbook fields; no missing keys or glyphs')
