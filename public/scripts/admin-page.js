function openSection(id){
    const c = document.getElementById('left-wing').children;
    console.log(c);
    for(let i = 0; i < c.length; i++){
        c[i].style.display = 'none';
    }
    document.getElementById(id).style.display = 'block';
}
async function createBoard(){
    const err = document.getElementById('board-err');
    const name = document.getElementById('board-name').value;
    const path = document.getElementById('board-path').value;
    const rules = document.getElementById('board-rules').value ?? '';

    if(!path.match(/\/[a-b0-9]/)){
        err.innerHTML = "ERROR: Path is not vaild. Should be of the form: '/{path}'."
    } else if(name.trim().length == 0){
        err.innerHTML = "ERROR: Name must exist."
    } else {
        err.innerHTML = "";
    }

    const res = await fetch("/api/board", {
        method: 'POST',
        headers: {
            'Authorization' : `Bearer ${sessionStorage.getItem('jwt')}`
        },
        body:JSON.stringify({
            name: name,
            path: path,
            rules: rules,
        }),
    });

    try {

        const json = await res.json();
        if(!json.ok){
            err.innerHTML = `Error ${res.status}: ${json.error}`;
        } else {
            err.innerHTML = `<span style="color: green;">Success!</span>`;
        }
        
    } catch (error) {
        alert("An error occured: "+error);
    } 
}