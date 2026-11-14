// fetch('/api/board', {
//     method: 'GET',
//     headers:{
//         'Authorization' : `Bearer ${sessionStorage.getItem('jwt')}`,
//     }
// }).then((res)=>{
//     return res.json();
// }).then((json)=>{
//     if(json.ok != true){
//         alert(json.error);
//     } else {

//     }
// });


for(let i = 0; i < 10; i++){
    insertBoard(i, i, `name-${i}`, 'note', 'rules');
}

function insertBoard(id, path, name, note, rules){
    const arr = document.getElementsByClassName('board-array')[0];
    arr.insertAdjacentHTML('beforeend', `
        <board id='board-${id}' onclick=link('/${path}')>
            <h3>${name}</h3>
            <span>${note}</span>
        </board>
        `);
}

function link(url){
    window.location = url;
}