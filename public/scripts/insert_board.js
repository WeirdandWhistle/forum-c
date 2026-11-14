let currentHoverTimeout;
let prevStyle = null;
const longHoverTimeMS = 1000;

function insertBoard(id, path, name, note, rules, arrayid=null){

    let arr;
    if(!arrayid) arr = document.getElementsByClassName('board-array')[0];
    else arr = document.getElementById(arrayid);
    arr.insertAdjacentHTML('beforeend', `
        <board style='z-index: 1;' id='board-${id}' onclick=link('/${path}')>
            <h3>${name}</h3>
            <span>${note}</span>
        </board>
        `);

    // const element = document.getElementById(`board-${id}`);
    // element.addEventListener('mouseenter', ()=>{
    //     console.log(element.style);
    //     currentHoverTimeout = setTimeout(()=>{
    //         prevStyle = cloneCSS(element.style);
    //         element.style.zIndex = '100';
    //         element.style.backgroundColor = 'red';
    //     }, longHoverTimeMS);
    // });
    // element.addEventListener('mouseleave', ()=>{
    //     clearTimeout(currentHoverTimeout);
    //     console.log(prevStyle);
    //     if(prevStyle) {
    //         element.style = prevStyle;
    //         prevStyle = null;
    //     }
    // });
}
// function cloneCSS(css){
//     let styleClone = new CSSStyleSheet();
//     for (var i = 0; i < css.cssRules.length; i++) {
//         styleClone.insertRule(css.cssRules[i].cssText);
//     }
// }
// function setCSSEqual(incss, outcss){
//     for (var i = 0; i < incss.cssRules.length; i++) {
//         styleClone.insertRule(css.cssRules[i].cssText);
//     }
// }

function link(url){
    window.location = url;
}