let jwt = getCookie("auth_token");
async function get_jwt(){
    try {
        if(JSON.parse(decodeBase64URL(jwt.split(".")[1])).admin != true){
          console.log("jwt doesnt fit requirements")
            jwt = prompt("Please enter your admin jwt:");
            setCookie("auth_token", jwt, 90);
        }    
    } catch (error) {
      console.log("jwt errored: ", error);
        jwt = prompt("Please enter your admin jwt:");
        setCookie("auth_token", jwt, 90);
    }    
    verify_jwt();
}
get_jwt();

async function verify_jwt() {
    const res = await fetch("/api/verify/"+jwt);
    const json = await res.json();
    if(json.ok == false) {
        alert("Error: "+json.error);
        return;
    }
    if(json.data.valid == false){
        alert("JWT is invalid. Please try again");
        get_jwt();
        return;
    }

    sessionStorage.setItem('jwt', jwt);
}

function decodeBase64URL(base64){
  const uint8arr = Uint8Array.fromBase64(base64, {alphabet:"base64url"});
  return new TextDecoder().decode(uint8arr);
}

// https://www.w3schools.com/js/js_cookies.asp
function getCookie(cname) {
  let name = cname + "=";
  let decodedCookie = decodeURIComponent(document.cookie);
  let ca = decodedCookie.split(';');
  for(let i = 0; i <ca.length; i++) {
    let c = ca[i];
    while (c.charAt(0) == ' ') {
      c = c.substring(1);
    }
    if (c.indexOf(name) == 0) {
      return c.substring(name.length, c.length);
    }
  }
  return "";
}
function setCookie(cname, cvalue, exdays) {
  const d = new Date();
  d.setTime(d.getTime() + (exdays*24*60*60*1000));
  let expires = "expires="+ d.toUTCString();
  document.cookie = cname + "=" + cvalue + ";" + expires + ";path=/";
}