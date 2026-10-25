"""Create, reopen, update, failed-save and delete flows for all ten menus."""
import argparse
import ctypes
import hashlib
import json
import os
from pathlib import Path
import shutil
import subprocess
import tempfile

ROOT=Path(__file__).resolve().parents[1]

def main():
    ap=argparse.ArgumentParser();ap.add_argument('--exe',required=True);ap.add_argument('--output')
    args=ap.parse_args();exe=Path(args.exe).resolve()
    out=Path(args.output).resolve() if args.output else Path(tempfile.mkdtemp(prefix='tv36-lifecycle-'))
    out.mkdir(parents=True,exist_ok=True)
    env=os.environ.copy()
    if os.name=='nt':env['PATH']='C:/msys64/ucrt64/bin;'+env['PATH']
    source_hashes={p.name:hashlib.sha256(p.read_bytes()).hexdigest() for p in (ROOT/'Data').iterdir() if p.is_file()}
    results=[]
    def record(name,ok):
        results.append({'case':name,'pass':bool(ok)})
        (out/'results.json').write_text(json.dumps(results,indent=2),encoding='utf8')
        print(('PASS ' if ok else 'FAIL ')+name,flush=True)
    specs=[
        (1,'customers',['Acceptance','0987654998','acceptance@example.com','Ha Noi',''],5,['0987654997','','Changed',''],2,'0987654997'),
        (2,'settopboxes',['ACCEPTANCE-NEW','Maker','Model','25/10/2026','1',''],5,['','Changed','','','',''],2,'Changed'),
        (3,'channel_categories',['Acceptance category','Description',''],3,['Changed','Description',''],1,'Changed'),
        (4,'channels',['Acceptance channel','TL001','991','HD','https://example.com/new','1',''],3,['Changed','','','','','',''],1,'Changed'),
        (5,'schedules',['1','KENH001','Acceptance TV','2030-01-01 12:00','2030-01-01 13:00','Description',''],3,['','','Changed','','','',''],2,'Changed'),
        (6,'packages',['Acceptance package','100000','1','KENH001,KENH002','VOD001,VOD002',''],3,['Changed','','','','',''],1,'Changed'),
        (7,'contracts',['KH011','GOI001','STB011','25/10/2026','25/10/2026','12','Ha Noi','','1',''],3,['','','','Changed','',''],7,'Changed'),
        (8,'hoadon',['HD001','100000','01/11/2026','',''],5,['120000','','',''],3,'120000'),
        (9,'noidungvod',['Acceptance VOD','Drama','90','Description','2024','1','998991','1','1',''],5,['Changed','','','','','','','',''],1,'Changed'),
        (10,'support_tickets',['HD001','','Acceptance issue','1',''],3,['Changed','','',''],4,'Changed'),
    ]
    for uc,file,create,update_op,update,field,value in specs:
        wd=out/f'uc{uc:03}';shutil.copytree(ROOT/'Data',wd/'Data')
        path=wd/'Data'/(file+'.txt')
        rows=lambda:[line.split('|') for line in path.read_text(encoding='utf8').splitlines() if line]
        def run(label,op,entries):
            text='\n'.join([str(uc),str(op),*entries,*(['0']*6)])+'\n'
            with (out/f'uc{uc:03}-{label}.log').open('w',encoding='utf8') as log:
                p=subprocess.run([str(exe)],input=text,text=True,encoding='utf8',env=env,cwd=wd,stdout=log,stderr=subprocess.STDOUT,timeout=8)
            return p.returncode,(out/f'uc{uc:03}-{label}.log').read_text(encoding='utf8',errors='replace')
        before=rows();code,log=run('create',1,create);after=rows()
        added=[r for r in after if r[0] not in {x[0] for x in before}]
        record(f'uc{uc:03}-create',code==0 and len(added)==1)
        if len(added)!=1:continue
        key=added[0][0]
        read=['1','','0'] if uc in [3,6,7,10] else ['1',''] if uc in [4,5] else ['']
        saved=path.read_bytes();code,log=run('reopen',2,read)
        record(f'uc{uc:03}-reopen',code==0 and key in log and path.read_bytes()==saved)
        code,log=run('update',update_op,[key,*update])
        record(f'uc{uc:03}-update',code==0 and next(r for r in rows() if r[0]==key)[field]==value)
        if os.name=='nt':
            saved=path.read_bytes()
            attempted={'0987654997':'0987654996','Changed':'Unsaved change','120000':'130000'}[value]
            failed_update=[attempted if entry==value else entry for entry in update]
            assert failed_update != update
            if not ctypes.windll.kernel32.SetFileAttributesW(str(path),1):raise RuntimeError('Cannot lock fixture')
            try:code,log=run('readonly-update',update_op,[key,*failed_update])
            finally:ctypes.windll.kernel32.SetFileAttributesW(str(path),128)
            record(f'uc{uc:03}-readonly-update',code==0 and path.read_bytes()==saved and any(s in log.lower() for s in ['error','failed','cannot']))
        delete_op=6 if uc in [1,2,8,9] else 4
        saved=path.read_bytes();code,log=run('cancel-delete',delete_op,[key,'n',''])
        record(f'uc{uc:03}-cancel-delete',code==0 and path.read_bytes()==saved)
        code,log=run('delete',delete_op,[key,'y',''])
        record(f'uc{uc:03}-delete',code==0 and key not in {r[0] for r in rows()})
    record('source-data-unchanged',source_hashes=={p.name:hashlib.sha256(p.read_bytes()).hexdigest() for p in (ROOT/'Data').iterdir() if p.is_file()})
    print(json.dumps({'passed':sum(r['pass'] for r in results),'total':len(results)}))
    return int(any(not r['pass'] for r in results))

if __name__=='__main__':raise SystemExit(main())
