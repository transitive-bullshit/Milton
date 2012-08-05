/* 
 * @auth: Travis Fischer
 * @acct: tfischer
 * @date: Spring 2009
 * @file: test.js
 */

function loadScript(url) {
   netscape.security.PrivilegeManager.enablePrivilege("UniversalXPConnect");
   var loader = Components.classes["@mozilla.org/moz/jssubscript-loader;1"]
                .getService(Components.interfaces.mozIJSSubScriptLoader);
   
   loader.loadSubScript(url, null);
}

function getStrings(val) {
   if (typeof(val) == "string") {
      var ret = [];
      
      ret.push(val);
      return ret;
   } else { // array of strings
      return val;
   }
}

var _type = { };
var _pre  = "";
var _post = "";
var _bot  = "";
var _top  = "";
var _links = [];

function process(obj, inVariant, p, struct, depth) {
   var pre = "", post = "";
   var brief = false;
   var info = "";
   var usage = "";
   var br = "<br>";
   
   var variants = [];
   
   for(var key in obj) {
      var name = key;
      
      if (key[0] == '#' || inVariant) {
         if (key[0] == '_')
            alert("error");
         
         if (key[0] == '#')
            name = key.substring(1);
         
         _links.push({"name" : name, "type" : "struct"});
         
         process(obj[key], false, name, true, depth + 1);
         post += _pre;
         post += "struct " + name + (inVariant ? " : public " + p : "") + " {" + br;
         post += "<div style=\"margin-left: 1em\">" + _post + "</div>";
         post += "};" + br + br;
         if (inVariant)
            variants.push(name);
      } else if (key[0] == '_') {
         name = key.substring(1);
         
         if (!brief) {
            pre += "/**" + br;
            brief = true;
         }
         
         if (name == "brief" || (name == "desc" && !("_brief" in obj))) {
            pre += "@brief " + obj[key] + br + br;
         } else if (name == "desc") {
            pre += obj[key] + br + br;
         } else if (name == "note") {
            pre += "@note " + obj[key] + br + br;
         } else if (name == "info") {
            info += "@htmlonly" + br;
            info += "&lt;table class='info'&gt;" + br;
            
            var i = obj[key];
            if ("type" in i) {
               var type = i["type"].toString();
               
               if (type != "node") {
                  if (type == "uint")
                     type = "unsigned int";
                  
                  var t = type;
                  if (type == "unsigned int") {
                     t = "unsigned integer";
                  } else if (type == "real") {
                     t = "real scalar";
                  } else if (type == "real+") {
                     t = "positive real scalar";
                  } else if (type.substring(0, 5) != "array" && 
                     type.substring(0, 6) != "vector" && 
                     type.substring(0, 6) != "string" && 
                     type.substring(0, 7) != "boolean")
                  {
                     // make typename into a link
                     t = "&lt;a href=\"struct" + t + ".php\"&gt;" + t + "&lt;/a&gt;";
                  }
                  
                  info += "&lt;tr&gt;&lt;td&gt;type&lt;/td&gt;&lt;td&gt;" + t + "&lt;/td&gt;&lt;/tr&gt;" + br;
                  
                  if (type == "boolean")
                     type = "bool";
                  else if (type == "real")
                     type = "real_t";
                  else if (type == "real+")
                     type = "ureal_t";
               }
               
               _type[p] = type;
            }
            
            if ("optional" in i) {
               info += "&lt;tr&gt;&lt;td&gt;optional&lt;/td&gt;&lt;td&gt;" + i["optional"].toString() + "&lt;/td&gt;&lt;/tr&gt;" + br;
            }
            if ("occurrences" in i) {
               info += "&lt;tr&gt;&lt;td&gt;occurrences&lt;/td&gt;&lt;td&gt;" + i["occurrences"].toString() + "&lt;/td&gt;&lt;/tr&gt;" + br;
            }
            if ("default" in i) {
               info += "&lt;tr&gt;&lt;td&gt;default&lt;/td&gt;&lt;td&gt;" + i["default"].toString() + "&lt;/td&gt;&lt;/tr&gt;" + br;
            }
            if ("units" in i) {
               info += "&lt;tr&gt;&lt;td&gt;units&lt;/td&gt;&lt;td&gt;" + i["units"].toString() + "&lt;/td&gt;&lt;/tr&gt;" + br;
            }
            if ("cond" in i) {
               info += "&lt;tr&gt;&lt;td&gt;restrictions&lt;/td&gt;&lt;td&gt;" + i["cond"].toString() + "&lt;/td&gt;&lt;/tr&gt;" + br;
            }
            
            info += "&lt;/table&gt;" + br + "@endhtmlonly" + br;
         } else if (name.substring(0, 5) == "usage") {
            usage += "@htmlonly" + br;
            usage += "&lt;div class='usage'&gt;" + br;
            usage += "&lt;p&gt;Example usage:&lt;/p&gt;" + br;
            
            usage += "&lt;div class='example'&gt;" + br;
            usage += obj[key] + br;
            usage += "&lt;/div&gt;";
            
            usage += "&lt;/div&gt;" + br;
            usage += "@endhtmlonly" + br;
         } else if (name == "class") {
            pre += "@note " + p + " corresponds to the underlying Milton class '" + obj[key] + ".'" + br + br;
         } else if (name == "image") {
            var g = obj[key];
            
            pre += "@htmlonly" + br;
            pre += "&lt;div class='gallery'&gt;" + br; 
            pre += "&lt;table&gt;" + br;
            pre += "&lt;tr&gt;" + br;
            var images = getStrings(g["_path"]);
            
            for(var i = 0; i < images.length; i++) {
               var f = images[i];
               pre += "&lt;td&gt;&lt;a href=\"" + f + "\"&gt;&lt;img width=\"128\" height=\"128\" src=\"" + f + "\" /&gt;&lt;/a&gt;&lt;/td&gt;";
               pre += br;
            }
            
            pre += "&lt;/tr&gt;" + br;
            pre += "&lt;/table&gt;" + br;
            
            if ("_caption" in g) {
               pre += "&lt;p&gt;";
               pre += g["_caption"];
               pre += "&lt;/p&gt;" + br;
            }
            
            pre += "&lt;/div&gt;" + br;
            pre += "@endhtmlonly" + br;
         } else if (name == "type") {
            var v = process(obj[key], true, p, true, depth + 1);
            
            _top += "struct " + p + ";" + br; // forward-declaration
            
            pre += "@htmlonly" + br;
            pre += "&lt;div class='variants'&gt;" + br; 
            pre += "&lt;p&gt;" + br;
            pre += "Available " + p + " variants: ";
            pre += "&lt;/p&gt;" + br;
            pre += "@endhtmlonly" + br;
            
            for(var i = 0; i < v.length; i++) {
               pre += v[i];
               if (i < v.length - 1)
                  pre += ", ";
            }
            
            pre += br;
            pre += "@htmlonly" + br;
            pre += "&lt;/div&gt;" + br;
            pre += "@endhtmlonly" + br;
         } else {
            alert("unknown key '" + key + "' encountered!");
         }
      } else if (key[0] == '%') {
         name = key.substring(1);
         
         _links.push({"name" : name, "type" : "namespace"});
         
         process(obj[key], false, name, false, depth + 1);
         post += _pre;
         post += "namespace " + name + " {" + br;
         post += "<div style=\"margin-left: 1em\">" + _post + "</div>";
         post += "};" + br + br;
         _top += "using namespace " + name + ";" + br;
      } else {
         process(obj[key], false, key, true, depth + 1);
         post += _pre;
         
         var type = key;
         if (key in _type)
            type = _type[key];
         
         if (type == "node") {
            post += "struct " + type + ";" + br + br;
         } else {
            post += type + " " + name + ";" + br + br;
         }
      }
   }
   
   pre += info + usage;
   
   if (brief)
      pre += "*/" + br;
   var txt = "<div>" + pre + post + "</div>";
   _pre  = pre;
   _post = post;
   
   if (inVariant) {
      _bot += txt;
      return variants;
   } else {
      return txt;
   }
}

// Add links to all htmlonly scenefile examples
function addLinks(txt, links) {
   for(var i = 0; i < links.length; i++) {
      var cur  = links[i];
      
      var name = cur["name"];
      var type = cur["type"];
      
      var reg  = "\"" + name + "\"";
      var rep  = "&lt;a href=\"" + type + name + ".php\"&gt;" + reg + "&lt;/a&gt;";
      
      txt = txt.replace(new RegExp(reg, "g"), rep);
   }
   
   return txt;
}

$(window).ready(function () {
   var out = $("#out");
   var txt = process(format, false, "root", false, 0);
   var br = "<br>";
   
   var head = "/* Autogenerated via a script by Travis Fischer (fisch0920@gmail.com)" + br + " * Do NOT edit this file itself, as your edits will get overwritten!!" + br + " */" + br + br;
   
   _top  = head + _top;
   _top += br;
   _top += "#include &lt;string&gt;" + br;
   _top += "using namespace std;" + br;
   _top += "typedef double* vector2;" + br;
   _top += "typedef double* vector3;" + br;
   _top += "typedef double* vector6;" + br;
   _top += "typedef double* vector9;" + br;
   _top += "typedef double* vector12;" + br;
   _top += "typedef double real_t;" + br;
   _top += "typedef double ureal_t;" + br;
   txt = _top + br + br + _bot + br + br + txt;
   
   txt = addLinks(txt, _links);
   out.append(txt);
});

