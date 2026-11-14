fetch('/api/board', {
    method: 'GET',
    headers:{
        'Authorization' : `Bearer ${sessionStorage.getItem('jwt')}`,
    }
}).then((res)=>{
    return res.json();
}).then((json)=>{
    if(json.ok != true){
        alert("Error: " + json.error);
    } else {
        for(const d of json.data) insertBoard(d.id, d.path, d.name, d.note, d.rules);
    }
});


// for(let i = 0; i < 10; i++){
//     insertBoard(i, i, `name-${i}`, 'note', 'rules');
// }