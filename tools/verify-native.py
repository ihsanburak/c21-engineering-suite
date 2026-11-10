"""Verify ARM ELF, Zehn structure and the exact official loader concatenation."""
import argparse,hashlib,json,struct
from pathlib import Path
p=argparse.ArgumentParser();p.add_argument('--loader',required=True,type=Path);a=p.parse_args()
root=Path(__file__).resolve().parents[1];base=root/'build/c21-engineering-suite'
elf=base.with_suffix('.elf').read_bytes()
assert elf[:6]==b'\x7fELF\x01\x01','Expected little-endian ELF32'
kind,machine=struct.unpack_from('<HH',elf,16)
assert kind==2 and machine==40,'Expected ARM executable'
zehn=Path(str(base)+'.tns.zehn').read_bytes();header=struct.unpack_from('<8I',zehn)
assert header[0]==0x6e68655a and header[1]==1,'Invalid Zehn signature/version'
assert header[2]==len(zehn) and header[6]>=len(zehn),'Invalid Zehn sizes'
tns=base.with_suffix('.tns').read_bytes();loader=a.loader.read_bytes()
assert tns==loader+zehn,'Final TNS does not match official loader plus Zehn payload'
evidence=dict(elf_class='ELF32',endianness='little',machine='ARM',
              zehn_version=header[1],zehn_relocations=header[3],zehn_flags=header[4],
              loader_allocation_bytes=header[6],official_loader_bytes=len(loader),
              tns_bytes=len(tns),sha256=hashlib.sha256(tns).hexdigest(),
              runtime_tested=False)
(root/'build/native-verification.json').write_text(json.dumps(evidence,indent=2)+'\n')
print(json.dumps(evidence,indent=2))
