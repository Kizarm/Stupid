#include <stdio.h>
#include <string>
#include "restore.h"
using namespace std;
static const char * js_text = R"---(var gWASM = null;  // globální proměnná + elementy stránky
const Outs = document.getElementById('output');
// async/await z příkladu na webu
(async () => {
  Outs.value = 'Compiling ...\n';        // presets
  // Build WebAssembly instance - WebAssembly.instantiateStreaming problem
  const memory = new WebAssembly.Memory({ initial: 32 });
  const importObject = {
    env: { memory },      // nutné pro práci a pamětí
    imports: {            // importované funkce do wasm
      PrintOut : (ptr, len) => {
        // pohled do paměti - ptr je vlastně číslo
        const view = new Uint8Array (memory.buffer, ptr, len);
        const utf8decoder = new TextDecoder();
        Outs.value += utf8decoder.decode(view); // to String
      },
      memoryGrow :  (len) => {
        console.log ('Growing the memory by ' + len.toString() + '. 64K blocks');
        memory.grow (len); // patrně to jde volat přímo z C/C++ kódu, ale tohle funguje
      },
      JsGetMiliseconds : () => {
        return Date.now();
      },
      JSDisplay : (ptr, len) => {
        const view = new Uint8Array (memory.buffer, ptr, len);
        const utf8decoder = new TextDecoder();
        document.getElementById('Display').innerHTML = utf8decoder.decode(view); // to String
      },
      JsSetOutput : (port, val) => {
        const id = 'Y' + port.toString();
        const el = document.getElementById(id);
        if (val) {
          el.style.backgroundColor = 'red';
        } else {
          el.style.backgroundColor = '#FFFFC0';
        }
      },
      JsAnalogOutput : (port, val) => {
        const id = 'AO' + port.toString();
        const el = document.getElementById(id);
        el.innerHTML   = val.toString();
        const wa = val * 100 / 256;
        el.style.width = wa + '%';
      },
    },
  };
  const response = await fetch('./$');
  const bytes    = await response.arrayBuffer();
  const module   = await WebAssembly.instantiate(bytes, importObject);
  gWASM = {
    asm : module.instance.exports,
    mem : memory,
  };
  gWASM.asm.init(memory.buffer.byteLength);
  
  MainLoop ();
})();
function MainLoop () {
  gWASM.asm.Loop ();
  window.setTimeout (MainLoop, 20);
}
var TableInput = [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0];
function Input (obj, n) {
  if (TableInput[n]) {
    obj.style.backgroundColor = '#FFFFC0';
    TableInput[n] = 0;
    gWASM.asm.DigitalInput (n, 0);
  } else {
    obj.style.backgroundColor = 'red';
    TableInput[n] = 1;
    gWASM.asm.DigitalInput (n, 1);
  }
};
function AnalogInput (obj, n) {
  gWASM.asm.AnalogInput (n, parseInt(obj.value));
};
)---";
// from context.s
extern "C" const char     libwasm [];
extern "C" const unsigned libwasm_size;
extern "C" const char     index_html [];
extern "C" const unsigned index_html_size;
extern "C" const char     symbols_txt [];
extern "C" const unsigned symbols_txt_size;
/////////////////
struct filedesc {
  const char   * context;
  const unsigned len;
  const char   * name;
};
static const filedesc files [] = {
  { .context = libwasm,     .len = libwasm_size,     .name = "libWASM.a"   },
  { .context = index_html,  .len = index_html_size,  .name = "index.html"  },
  { .context = symbols_txt, .len = symbols_txt_size, .name = "symbols.txt" },
};
static void save_file (const filedesc & desc) {
  FILE * out = fopen (desc.name, "w");
  if (!out) return;
  fwrite (desc.context, 1, desc.len, out);
  fclose (out);
}
static void save_js (const char * name) {
  string js (js_text);
  size_t n = js.find ('$');
  if (n == string::npos) return;
  string res = js.substr (0, n);
  res += name;
  // res += ".wasm";
  res += js.substr (n+1);
  FILE * out = fopen ("index.js", "w");
  if (!out) return;
  fwrite (res.c_str(), 1, res.length(), out);
  fclose (out);
}

void restore_files (const char * name) {
  for (const auto & e: files) {
    save_file (e);
  }
  save_js (name);
}
