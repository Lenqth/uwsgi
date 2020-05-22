NAME = 'jwsgi'

jvm_path = 'plugins/jvm'

up = {}
try:
    exec(compile(open('%s/uwsgiplugin.py' % jvm_path, "rb").read(), '%s/uwsgiplugin.py' % jvm_path, 'exec'), up)
except Exception:
    f = open('%s/uwsgiplugin.py' % jvm_path)
    exec(f.read(), up)
    f.close()

CFLAGS = up['CFLAGS']
CFLAGS.append('-I%s' % jvm_path)
LDFLAGS = []
LIBS = []
GCC_LIST = ['jwsgi_plugin']
