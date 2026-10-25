"""Shared input and ID persistence regressions on isolated data copies."""
import argparse
import hashlib
import json
import os
from pathlib import Path
import shutil
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]

UNIT = r'''
#ifdef _WIN32
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
#include "Repositories/Repository.h"
#include "Entities/Customer.h"
#include <iostream>
int main() {
    int failures=0;
    auto check=[&](bool ok,const char* name){std::cout<<(ok?"PASS ":"FAIL ")<<name<<"\n";if(!ok)++failures;};
    auto put=[](const char* path,const char* value){std::ofstream f(path);f<<value;};
    put("customers.txt","KH001|Existing|0987654321|a@example.com|Ha Noi\n");
    put("counter.txt","0");
    Repository<Customer> collision("customers.txt","counter.txt","KH"); collision.loadFromFile();
    check(collision.generateNextID()=="KH002","skip existing ID after stale counter");
    put("counter.txt","999");
    Repository<Customer> exhausted("customers.txt","counter.txt","KH"); exhausted.loadFromFile();
    bool rejected=false;try{exhausted.generateNextID();}catch(const std::exception&){rejected=true;}
    std::ifstream input("counter.txt");std::string value;input>>value;input.close();
    check(rejected && value=="999","exhaustion leaves counter unchanged");
    for(const char* bad:{"-1","junk","2147483647","1abc"}) {
        put("counter.txt",bad);rejected=false;
        try{Repository<Customer> r("customers.txt","counter.txt","KH");}catch(const std::exception&){rejected=true;}
        check(rejected,"invalid counter rejected");
    }
#ifdef _WIN32
    put("counter.txt","0");put("empty.txt","");
    Repository<Customer> locked("empty.txt","counter.txt","KH");locked.loadFromFile();
    check(SetFileAttributesW(L"counter.txt",FILE_ATTRIBUTE_READONLY)!=0,"set readonly counter");
    rejected=false;try{locked.generateNextID();}catch(const std::exception&){rejected=true;}
    SetFileAttributesW(L"counter.txt",FILE_ATTRIBUTE_NORMAL);
    check(rejected,"counter write error must reject ID");
    check(locked.generateNextID()=="KH001","failed counter write must not advance RAM count");
#endif
    return failures?1:0;
}
'''


def main():
    ap=argparse.ArgumentParser()
    ap.add_argument('--exe',required=True)
    ap.add_argument('--output')
    args=ap.parse_args()
    exe=Path(args.exe).resolve()
    out=Path(args.output).resolve() if args.output else Path(tempfile.mkdtemp(prefix='tv36-storage-'))
    out.mkdir(parents=True,exist_ok=True)
    env=os.environ.copy()
    if os.name=='nt':env['PATH']='C:/msys64/ucrt64/bin;'+env['PATH']
    results=[]
    def record(name,ok,detail=''):
        results.append({'case':name,'pass':bool(ok),'detail':detail})
        print(('PASS ' if ok else 'FAIL ')+name,flush=True)
        (out/'results.json').write_text(json.dumps(results,indent=2),encoding='utf8')
    cases={'eof-main':[], 'eof-submenu':['1'], 'eof-create-name':['1','1'],
           'eof-create-phone':['1','1','Temporary'],
           'eof-partial-update':['1','5','KH001','0987654999'],
           'eof-vod-duration':['9','1','Temporary','Drama'],
           'eof-schedule':['5','1','2','KENH001','VOD014'],
           'eof-contract-extension':['7','3','HD001','','','1'],
           'eof-delete-confirmation':['9','6','VOD020']}
    for name,entries in cases.items():
        wd=out/name;shutil.copytree(ROOT/'Data',wd/'Data')
        digest=lambda:{p.name:hashlib.sha256(p.read_bytes()).hexdigest() for p in (wd/'Data').iterdir() if p.is_file() and 'counter' not in p.name}
        before=digest()
        text='\n'.join(entries)+('\n' if entries else '')
        try:
            # Discard output for these cases: the old EOF loop prints indefinitely.
            p=subprocess.run([str(exe)],input=text,text=True,cwd=wd,env=env,stdout=subprocess.DEVNULL,stderr=subprocess.DEVNULL,timeout=2)
            record(name,p.returncode==0 and digest()==before,f'exit={p.returncode}; unchanged={digest()==before}')
        except subprocess.TimeoutExpired:
            record(name,False,'program loops after input closes')
    unitdir=out/'id-unit';unitdir.mkdir()
    command=[shutil.which('g++',path=env['PATH']),'-std=c++17','-I',str(ROOT),'-x','c++','-',str(ROOT/'Entities/Customer.cpp')]
    command += [str(p) for p in (ROOT/'Utils').glob('*.cpp')]
    command += ['-o',str(out/'id-tests.exe')]
    p=subprocess.run(command,input=UNIT,text=True,capture_output=True,env=env,timeout=60)
    if p.returncode:raise RuntimeError(p.stderr)
    p=subprocess.run([str(out/'id-tests.exe')],cwd=unitdir,text=True,capture_output=True,env=env,timeout=10)
    record('id-persistence',p.returncode==0,p.stdout+p.stderr)
    print(json.dumps({'passed':sum(r['pass'] for r in results),'total':len(results)}))
    return int(any(not r['pass'] for r in results))

if __name__=='__main__':raise SystemExit(main())
