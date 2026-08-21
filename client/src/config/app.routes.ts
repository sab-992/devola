import { Routes } from '@angular/router';
import { AuthenticationPage } from '@components/authentication/authentication'
import { FeedPage } from '@components/feed/feed';
import { RecommendationPage } from '@components/recommendation/recommendation';
import { authGuard } from '@guards/auth/auth-guard';
import { nonAuthGuard } from '@guards/non-auth/non-auth-guard';


export const routes: Routes = [{ path: "",               component: AuthenticationPage, canActivate: [nonAuthGuard] },
                               { path: "feed",           component: FeedPage,           canActivate: [authGuard] },
                               { path: "recommendation", component: RecommendationPage, canActivate: [authGuard] },
                               // TODO: replace redirect to the 404 not found page.
                               { path: "not-found", redirectTo: "" },
                               { path: "**", redirectTo: "not-found" }];
