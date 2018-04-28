
(function(kk){
	var modules = {};
	kk.require = function(path) {
		var m = modules[path];
		if(m === undefined) {
			m = { exports : {} };
			try {
				var code = kk.getString(path);
				var fn = eval("(function(module,exports){" + code + "})");
				if(typeof fn == 'function') {
					fn(m,m.exports);
				}
				print("require " + path);
			} catch(e) {
				print(e.toString());
			}
			modules[path] = m;
		}
		return m.exports;
	};
})(kk);
