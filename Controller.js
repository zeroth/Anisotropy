function createImage(path) {

    Manager.createImage(path);
//    viewer.bioImgViewer.visible = true;
//    viewer.bioImgViewer.setImage(img);
//    let imgViewer = bioImgViewer.createObject();
//    imgViewer.setImage(img);
//    viewer.scrollView.contentChildren.push(img);
}

function applyAnisotropy(parlBgFile, perpBgFile, parlFile, perpFile, subtractValue) {
//    console.log(parlBgFile, perpBgFile, parlFile, perpFile, subtractValue);
//    let an = ani.createObject();

    ani.parallelBackGround = parlBgFile;
    ani.perpendicularBackground = perpBgFile;
    ani.parallel = parlFile;
    ani.perpendicular = perpFile;
    ani.subtractVal = subtractValue;
//    ani.setImageViewer(viewer.bioImgViewer);
    ani.apply();
}
