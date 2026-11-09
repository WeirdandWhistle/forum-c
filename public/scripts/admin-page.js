function openSection(id){
    const c = document.getElementById('left-wing').children;
    console.log(c);
    for(let i = 0; i < c.length; i++){
        c[i].style.display = 'none';
    }
    document.getElementById(id).style.display = 'block';
}
function createBoard(){
    const err = document.getElementById('board-err');
    const name = document.getElementById('board-name').value;
    const path = document.getElementById('board-path').value;
    const rules = document.getElementById('board-rules').value;

    if(!path.match(/\/[a-b0-9]/)){
        err.innerHTML = "ERROR: Path is not vaild. Should be of the form: '/{path}'."
    } else if(name.trim().length == 0){
        err.innerHTML = "ERROR: Name must exist."
    } else {
        err.innerHTML = "";
    }

    const res = fetch("/api");
}